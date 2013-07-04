#include <gtest/gtest.h>

#include <boost/lexical_cast.hpp>

#include <qi/application.hpp>

#include <qitype/objectfactory.hpp>

#include <qimessaging/session.hpp>
#include <testsession/testsessionpair.hpp>


/* For asynchronous things where no synchronisation mechanism
 * is possible, loop the check and wait a small delay,
 * instead of one big sleep that will slow us down
 *
 */
#define PERSIST_CHECK(code, cond, what, msdelay)  \
do                                           \
{                                            \
  code;                                      \
  for(unsigned i=0; i<50 && !(cond); ++i)    \
  {                                          \
    qi::os::msleep(1 + msdelay / 50);        \
    code;                                    \
  }                                          \
  what(cond);                                \
} while(0)

#define PERSIST_ASSERT(code, cond, msdelay)  \
 PERSIST_CHECK(code, cond, ASSERT_TRUE, msdelay)
#define PERSIST_EXPECT(code, cond, msdelay)  \
 PERSIST_CHECK(code, cond, EXPECT_TRUE, msdelay)
#define PERSIST(code, cond, msdelay)  \
 PERSIST_CHECK(code, cond, ,msdelay)

std::vector<qi::AnyValue> arguments(qi::AutoAnyReference v1 = qi::AutoAnyReference(),
  qi::AutoAnyReference v2 = qi::AutoAnyReference(),
  qi::AutoAnyReference v3 = qi::AutoAnyReference(),
  qi::AutoAnyReference v4 = qi::AutoAnyReference(),
  qi::AutoAnyReference v5 = qi::AutoAnyReference())
{
  std::vector<qi::AnyValue> res;
  if (v1.value)
    res.push_back(qi::AnyValue(v1));
  if (v2.value)
    res.push_back(qi::AnyValue(v2));
  if (v3.value)
    res.push_back(qi::AnyValue(v3));
  if (v4.value)
    res.push_back(qi::AnyValue(v4));
  if (v5.value)
    res.push_back(qi::AnyValue(v5));
  return res;
}


class TestObject
{
public:
  int getv() { return v.get();}
  void setv(int i) { v.set(i);}
  int add(int i) { int res =  i+v.get(); lastRes = res; onAdd(res); return res;}
  int lastAdd() { return lastRes;}
  qi::Property<int> v;
  qi::Signal<int> onAdd;
  int lastRes;
};
QI_REGISTER_OBJECT(TestObject, add, getv, setv, v, onAdd, lastAdd);


class TestObject2 // yes its the same
{
public:
  int getv() { return v.get();}
  void setv(int i) { v.set(i);}
  int add(int i) { int res =  i+v.get(); lastRes = res; onAdd(res); return res;}
  int lastAdd() { return lastRes;}
  qi::Property<int> v;
  qi::Signal<int> onAdd;
  int lastRes;
};
QI_REGISTER_OBJECT(TestObject2, add, getv, setv, v, onAdd, lastAdd);


// TestObjectService::create
QI_REGISTER_OBJECT_FACTORY_BUILDER(TestObject);
QI_REGISTER_OBJECT_FACTORY_CONSTRUCTOR(TestObject2);

#define STRING(a) std::string(#a)
TEST(Behavior, testFactory)
{
  TestSessionPair p;
  ASSERT_EQ(1u, p.server()->loadService("behavior").size());
  qi::AnyObject b = p.client()->service("BehaviorService").value()->call<qi::AnyObject>("create");
  b->call<void>("connect", p.serviceDirectoryEndpoints()[0].str());
  std::string behavior = STRING(
    a Whatever TestObjectService.create;
    b Whatever TestObjectService.create;
    c Whatever TestObject2;
    ab a.v -> b.add;
    bc b.onAdd -> c.add;
    );
  size_t pos = 0;
  while((pos = behavior.find_first_of(';', pos)) != behavior.npos)
    behavior[pos] = '\n';
  std::cerr << "INPUT: " << behavior << std::endl;
  b->call<void>("loadString", behavior);
  b->call<void>("loadObjects");
  b->call<void>("call", "a", "setv", arguments(42));
  ASSERT_EQ(42, b->call<int>("call", "a", "getv", arguments()));
  b->call<void>("call", "b", "setv", arguments(1));
  b->call<void>("call", "c", "setv", arguments(2));
  b->call<void>("setTransitions", false);
  ASSERT_EQ(2, b->call<int>("call", "c", "getv", arguments()));
  b->call<void>("call", "a", "setv", arguments(3));
  qi::os::msleep(1000);
  ASSERT_EQ(6, b->call<int>("call", "c", "lastAdd", arguments()));
}


TEST(Behavior, testService)
{
  TestSessionPair p;
  qi::os::dlopen("behavior");
  p.server()->registerService("BehaviorService", qi::createObject("BehaviorService"));
  qi::AnyObject b = p.client()->service("BehaviorService").value()->call<qi::AnyObject>("create");
  b->call<void>("connect", p.serviceDirectoryEndpoints()[0].str());
  p.server()->registerService("TestObject2", qi::createObject("TestObject2"));
  std::string behavior = STRING(
    a Whatever TestObjectService.create v=42;
    b Whatever TestObjectService.create v=1;
    c Whatever TestObject2              v=2;
    ab a.v -> b.add;
    bc b.onAdd -> c.add;
    );
  size_t pos = 0;
  while((pos = behavior.find_first_of(';', pos)) != behavior.npos)
    behavior[pos] = '\n';
  std::cerr << "INPUT: " << behavior << std::endl;
  b->call<void>("loadString", behavior);
  b->call<void>("loadObjects");
  ASSERT_EQ(42, b->call<int>("call", "a", "getv", arguments()));
  ASSERT_EQ(1, b->call<int>("call", "b", "getv", arguments()));
  ASSERT_EQ(2, b->call<int>("call", "c", "getv", arguments()));
  b->call<void>("setTransitions", false);
  b->call<void>("call", "a", "setv", arguments(3));
  PERSIST(, 6 == b->call<int>("call", "c", "lastAdd", arguments()), 1000);
  ASSERT_EQ(6, b->call<int>("call", "c", "lastAdd", arguments()));
  // recheck by accessing the service
  qi::AnyObject daC = p.client()->service("TestObject2");
  ASSERT_EQ(6, daC->call<int>("lastAdd"));
}


std::string str(const qi::AnyValue& v)
{
  if (v.kind() == qi::TypeKind_String)
    return v.toString();
  else
    return boost::lexical_cast<std::string>(v.toDouble());
}

void onTransition(
  std::vector<std::string>& transitionData,
  const std::string& transId,
  qi::AnyValue payload)
{
  transitionData.push_back(transId + " " + str(payload));
}

TEST(Behavior, transitionTrack)
{
  TestSessionPair p;
  qi::os::dlopen("behavior");
  p.server()->registerService("BehaviorService", qi::createObject("BehaviorService"));
  qi::AnyObject b = p.client()->service("BehaviorService").value()->call<qi::AnyObject>("create");
  b->call<void>("connect", p.serviceDirectoryEndpoints()[0].str());
  p.server()->registerService("TestObject2", qi::createObject("TestObject2"));
  std::string behavior = STRING(
    a Whatever TestObjectService.create;
    b Whatever TestObjectService.create;
    c Whatever TestObject2;
    d Whatever TestObjectService.create;
    ab a.v -> b.add;
    bc b.onAdd -> c.add;
    cd c.onAdd -> d.add;
    );
  size_t pos = 0;
  while((pos = behavior.find_first_of(';', pos)) != behavior.npos)
    behavior[pos] = '\n';
  std::cerr << "INPUT: " << behavior << std::endl;
  b->call<void>("loadString", behavior);
  b->call<void>("loadObjects");
  b->call<void>("call", "a", "setv", arguments(42));
  ASSERT_EQ(42, b->call<int>("call", "a", "getv", arguments()));
  b->call<void>("call", "b", "setv", arguments(1));
  b->call<void>("call", "c", "setv", arguments(2));
  b->call<void>("setTransitions", true);
  std::vector<std::string> transitionData;
  b->connect("onTransition",
    boost::function<void(const std::string&, qi::AnyValue)>
    (boost::bind(&onTransition, boost::ref(transitionData), _1, _2)));
  b->call<void>("call", "a", "setv", arguments(3));
  PERSIST(, 6 == b->call<int>("call", "c", "lastAdd", arguments()), 1000);
  ASSERT_EQ(6, b->call<int>("call", "c", "lastAdd", arguments()));
  ASSERT_EQ(3u, transitionData.size());
  std::sort(transitionData.begin(), transitionData.end());
  ASSERT_EQ("ab 3", transitionData[0]);
  ASSERT_EQ("bc 4", transitionData[1]);
  ASSERT_EQ("cd 6", transitionData[2]);
}


TEST(Behavior, targetPropertyDbgOn)
{
  TestSessionPair p;
  qi::os::dlopen("behavior");
  p.server()->registerService("BehaviorService", qi::createObject("BehaviorService"));
  qi::AnyObject b = p.client()->service("BehaviorService").value()->call<qi::AnyObject>("create");
  b->call<void>("connect", p.serviceDirectoryEndpoints()[0].str());
  p.server()->registerService("TestObject2", qi::createObject("TestObject2"));
  std::string behavior = STRING(
    a Whatever TestObjectService.create;
    b Whatever TestObjectService.create;
    c Whatever TestObject2;
    d Whatever TestObjectService.create;
    ab a.v -> b.v;
    bc b.v -> c.v;
    cd c.v -> d.v;
    );
  size_t pos = 0;
  while((pos = behavior.find_first_of(';', pos)) != behavior.npos)
    behavior[pos] = '\n';
  std::cerr << "INPUT: " << behavior << std::endl;
  b->call<void>("loadString", behavior);
  b->call<void>("loadObjects");
  b->call<void>("setTransitions", true);
  b->call<void>("call", "a", "setv", arguments(55));
  PERSIST(, 55 == b->call<int>("call", "d", "getv", arguments()), 1000);
  ASSERT_EQ(55, b->call<int>("call", "d", "getv", arguments()));
}

TEST(Behavior, targetPropertyDbgOff)
{
  TestSessionPair p;
  qi::os::dlopen("behavior");
  p.server()->registerService("BehaviorService", qi::createObject("BehaviorService"));
  qi::AnyObject b = p.client()->service("BehaviorService").value()->call<qi::AnyObject>("create");
  b->call<void>("connect", p.serviceDirectoryEndpoints()[0].str());
  p.server()->registerService("TestObject2", qi::createObject("TestObject2"));
  std::string behavior = STRING(
    a Whatever TestObjectService.create;
    b Whatever TestObjectService.create;
    c Whatever TestObject2;
    d Whatever TestObjectService.create;
    ab a.v -> b.v;
    bc b.v -> c.v;
    cd c.v -> d.v;
    );
  size_t pos = 0;
  while((pos = behavior.find_first_of(';', pos)) != behavior.npos)
    behavior[pos] = '\n';
  std::cerr << "INPUT: " << behavior << std::endl;
  b->call<void>("loadString", behavior);
  b->call<void>("loadObjects");
  b->call<void>("setTransitions", false);
  b->call<void>("call", "a", "setv", arguments(55));
  PERSIST(, 55 == b->call<int>("call", "d", "getv", arguments()), 1000);
  ASSERT_EQ(55, b->call<int>("call", "d", "getv", arguments()));
}

int main(int argc, char **argv) {
  qi::Application app(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  TestMode::initTestMode(argc, argv);
  return RUN_ALL_TESTS();
}
