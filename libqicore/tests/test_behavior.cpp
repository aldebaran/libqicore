#include <gtest/gtest.h>

#include <boost/assign/list_of.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <qi/application.hpp>
#include <qi/anyobject.hpp>
#include <qi/type/proxyproperty.hpp>

#include <qi/session.hpp>

#include <qicore/task.hpp>
#include <testsession/testsessionpair.hpp>
#include <qi/anymodule.hpp>

QI_TYPE_ENUM_REGISTER(qi::MetaCallType);

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
  if (v1.rawValue())
    res.push_back(qi::AnyValue(v1));
  if (v2.rawValue())
    res.push_back(qi::AnyValue(v2));
  if (v3.rawValue())
    res.push_back(qi::AnyValue(v3));
  if (v4.rawValue())
    res.push_back(qi::AnyValue(v4));
  if (v5.rawValue())
    res.push_back(qi::AnyValue(v5));
  return res;
}

template<typename T> class Pulse
{
public:
  Pulse(T& tgt, T start, T end)
  : tgt(tgt), end(end)
  {
    tgt = start;
  }
  ~Pulse()
  {
    tgt = end;
  }
  T& tgt;
  T end;
};

template<typename T> class Pulse<qi::Property<T> >
{
public:
  Pulse(qi::Property<T>& prop, T start, T end)
  :prop(prop), end(end)
  {
    prop.set(start);
  }
  ~Pulse()
  {
    prop.set(end);
  }
  qi::Property<T>& prop;
  T end;
};

class Sqrt
{
public:
  double sqrt(double v)
  {
    if (v < 0)
      throw std::runtime_error("Negative input");
    qi::os::msleep(v);
    return std::sqrt(v);
  }
};
QI_REGISTER_OBJECT(Sqrt, sqrt);

class SqrtTask: public qi::Task
{
public:
  virtual bool interrupt()
  {
    if (_running)
    {
      _running = false;
      return true;
    }
    else
      return false;
  }
  void start(double v)
  {
    qiLogInfo("SqrtTask") << "start " << v;
    Pulse<qi::Property<bool> > pulse(running, true, false);
    Pulse<bool> pulse2(_running, true, false);
    if (v < 0)
    {
      error.set("Negative input");
    }
    else
    {
      // hmm, that's a long computation...
      qi::os::msleep(v);
      if (!_running)
        error.set("interrupted");
      else
        val.set(std::sqrt(v));
    }
    qiLogInfo("SqrtTask") << "stop";
  }
  qi::Property<double> val;
  bool _running;
};

QI_REGISTER_OBJECT(SqrtTask, start, val, QI_TASK_MEMBERS);

class PropHolder
{
public:
  qi::Property<qi::AnyValue> prop;
  qi::Property<qi::AnyValue> prop2;
  qi::Property<int> propInt;
};

QI_REGISTER_OBJECT(PropHolder, prop, prop2, propInt);

class TestObject
{
public:
  TestObject() : lastRes(0) {}
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
  TestObject2() : lastRes(0) {}
  int getv() { return v.get();}
  void setv(int i) { v.set(i);}
  int add(int i) { int res =  i+v.get(); lastRes = res; onAdd(res); return res;}
  int lastAdd() { return lastRes;}
  qi::Property<int> v;
  qi::Signal<int> onAdd;
  int lastRes;
};
QI_REGISTER_OBJECT(TestObject2, add, getv, setv, v, onAdd, lastAdd);


void register_testbehav(qi::ModuleBuilder* mb) {
  mb->advertiseFactory<TestObject>("TestObjectService");
  mb->advertiseFactory<SqrtTask>("SqrtTaskService");
  mb->advertiseFactory<Sqrt>("SqrtService");

  mb->advertiseFactory<TestObject2>("TestObject2");
  mb->advertiseFactory<PropHolder>("PropHolder");

  // TestObjectService::create
  //QI_REGISTER_OBJECT_FACTORY_BUILDER(TestObject);
  //QI_REGISTER_OBJECT_FACTORY_BUILDER(SqrtTask);
  //QI_REGISTER_OBJECT_FACTORY_BUILDER(Sqrt);
  //QI_REGISTER_OBJECT_FACTORY_CONSTRUCTOR(TestObject2);
  //QI_REGISTER_OBJECT_FACTORY_CONSTRUCTOR(PropHolder);
}

QI_REGISTER_MODULE_EMBEDDED("testbehav", &register_testbehav);


#define STRING(a) std::string(#a)
TEST(Behavior, testFactory)
{
  TestSessionPair p;
  p.server()->loadService("qicore.Behavior");

  //qi::os::sleep(5000);
  qi::AnyObject b = p.client()->service("Behavior");
  //b.call<void>("connect", p.serviceDirectoryEndpoints()[0].str());
  std::string behavior = STRING(
    a Whatever testbehav.TestObjectService;
    b Whatever testbehav.TestObjectService;
    c Whatever testbehav.TestObject2;
    ab a.v -> b.add;
    bc b.onAdd -> c.add;
    );
  size_t pos = 0;
  while((pos = behavior.find_first_of(';', pos)) != behavior.npos)
    behavior[pos] = '\n';
  std::cerr << "INPUT: " << behavior << std::endl;
  b.call<void>("loadString", behavior);
  b.call<void>("loadObjects", false);
  b.call<void>("call", "a", "setv", arguments(42));
  ASSERT_EQ(42, b.call<int>("call", "a", "getv", arguments()));
  b.call<void>("call", "b", "setv", arguments(1));
  b.call<void>("call", "c", "setv", arguments(2));
  b.call<void>("setTransitions", false, qi::MetaCallType_Auto);
  ASSERT_EQ(2, b.call<int>("call", "c", "getv", arguments()));
  b.call<void>("call", "a", "setv", arguments(3));
  qi::os::msleep(1000);
  ASSERT_EQ(6, b.call<int>("call", "c", "lastAdd", arguments()));
}


TEST(Behavior, testService)
{
  TestSessionPair p;
  p.server()->loadService("qicore.Behavior");
  qi::AnyObject b = p.client()->service("Behavior");
  //b.call<void>("connect", p.serviceDirectoryEndpoints()[0].str());
  p.server()->loadService("testbehav.TestObject2");
  std::string behavior = STRING(
    a Whatever testbehav.TestObjectService v=42;
    b Whatever testbehav.TestObjectService v=1;
    c Whatever s.TestObject2              v=2;
    ab a.v -> b.add;
    bc b.onAdd -> c.add;
    );
  size_t pos = 0;
  while((pos = behavior.find_first_of(';', pos)) != behavior.npos)
    behavior[pos] = '\n';
  std::cerr << "INPUT: " << behavior << std::endl;
  b.call<void>("loadString", behavior);
  b.call<void>("loadObjects", false);
  ASSERT_EQ(42, b.call<int>("call", "a", "getv", arguments()));
  ASSERT_EQ(1,  b.call<int>("call", "b", "getv", arguments()));
  ASSERT_EQ(2,  b.call<int>("call", "c", "getv", arguments()));
  b.call<void>("setTransitions", false, qi::MetaCallType_Auto);
  b.call<void>("call", "a", "setv", arguments(3));
  PERSIST(, 6 == b.call<int>("call", "c", "lastAdd", arguments()), 1000);
  ASSERT_EQ(6, b.call<int>("call", "c", "lastAdd", arguments()));
  // recheck by accessing the service
  qi::AnyObject daC = p.client()->service("TestObject2");
  ASSERT_EQ(6, daC.call<int>("lastAdd"));
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
  p.server()->loadService("qicore.Behavior");
  qi::AnyObject b = p.client()->service("Behavior");
  //b.call<void>("connect", p.serviceDirectoryEndpoints()[0].str());
  p.server()->loadService("testbehav.TestObject2");
  std::string behavior = STRING(
    a Whatever testbehav.TestObjectService;
    b Whatever testbehav.TestObjectService;
    c Whatever s.TestObject2;
    d Whatever testbehav.TestObjectService;
    ab a.v -> b.add;
    bc b.onAdd -> c.add;
    cd c.onAdd -> d.add;
    );
  size_t pos = 0;
  while((pos = behavior.find_first_of(';', pos)) != behavior.npos)
    behavior[pos] = '\n';
  std::cerr << "INPUT: " << behavior << std::endl;
  b.call<void>("loadString", behavior);
  b.call<void>("loadObjects", false);
  b.call<void>("call", "a", "setv", arguments(42));
  ASSERT_EQ(42, b.call<int>("call", "a", "getv", arguments()));
  b.call<void>("call", "b", "setv", arguments(1));
  b.call<void>("call", "c", "setv", arguments(2));
  b.call<void>("setTransitions", true, qi::MetaCallType_Auto);
  std::vector<std::string> transitionData;
  b.connect("onTransition",
    boost::function<void(const std::string&, qi::AnyValue)>
    (boost::bind(&onTransition, boost::ref(transitionData), _1, _2)));
  b.call<void>("call", "a", "setv", arguments(3));
  PERSIST(, 6 == b.call<int>("call", "c", "lastAdd", arguments()), 1000);
  ASSERT_EQ(6, b.call<int>("call", "c", "lastAdd", arguments()));
  ASSERT_EQ(3u, transitionData.size());
  std::sort(transitionData.begin(), transitionData.end());
  ASSERT_EQ("ab 3", transitionData[0]);
  ASSERT_EQ("bc 4", transitionData[1]);
  ASSERT_EQ("cd 6", transitionData[2]);
}


TEST(Behavior, targetPropertyDbgOn)
{
  TestSessionPair p;
  p.server()->loadService("qicore.Behavior");
  qi::AnyObject b = p.client()->service("Behavior");
  //b.call<void>("connect", p.serviceDirectoryEndpoints()[0].str());
  p.server()->loadService("testbehav.TestObject2");
  std::string behavior = STRING(
    a Whatever testbehav.TestObjectService;
    b Whatever testbehav.TestObjectService;
    c Whatever s.TestObject2;
    d Whatever testbehav.TestObjectService;
    ab a.v -> b.v;
    bc b.v -> c.v;
    cd c.v -> d.v;
    );
  size_t pos = 0;
  while((pos = behavior.find_first_of(';', pos)) != behavior.npos)
    behavior[pos] = '\n';
  std::cerr << "INPUT: " << behavior << std::endl;
  b.call<void>("loadString", behavior);
  b.call<void>("loadObjects", false);
  b.call<void>("setTransitions", true, qi::MetaCallType_Auto);
  b.call<void>("call", "a", "setv", arguments(55));
  PERSIST(, 55 == b.call<int>("call", "d", "getv", arguments()), 1000);
  ASSERT_EQ(55, b.call<int>("call", "d", "getv", arguments()));
}

TEST(Behavior, targetPropertyDbgOff)
{
  TestSessionPair p;
  p.server()->loadService("qicore.Behavior");
  qi::AnyObject b = p.client()->service("Behavior");
  //b.call<void>("connect", p.serviceDirectoryEndpoints()[0].str());
  p.server()->loadService("testbehav.TestObject2");
  std::string behavior = STRING(
    a Whatever testbehav.TestObjectService;
    b Whatever testbehav.TestObjectService;
    c Whatever s.TestObject2;
    d Whatever testbehav.TestObjectService;
    ab a.v -> b.v;
    bc b.v -> c.v;
    cd c.v -> d.v;
    );
  size_t pos = 0;
  while((pos = behavior.find_first_of(';', pos)) != behavior.npos)
    behavior[pos] = '\n';
  std::cerr << "INPUT: " << behavior << std::endl;
  b.call<void>("loadString", behavior);
  b.call<void>("loadObjects", false);
  b.call<void>("setTransitions", false, qi::MetaCallType_Auto);
  b.call<void>("call", "a", "setv", arguments(55));
  PERSIST(, 55 == b.call<int>("call", "d", "getv", arguments()), 1000);
  ASSERT_EQ(55, b.call<int>("call", "d", "getv", arguments()));
}

TEST(Behavior, PropSet)
{
  TestSessionPair p;
  p.server()->loadService("qicore.Behavior");
  qi::AnyObject b = p.client()->service("Behavior");
  //b.call<void>("connect", p.serviceDirectoryEndpoints()[0].str());
  p.server()->loadService("testbehav.PropHolder");
  qi::AnyObject propHolder = p.client()->service("PropHolder");
  qi::ProxyProperty<qi::AnyValue> prop(propHolder, "prop");
  qi::ProxyProperty<qi::AnyValue> prop2(propHolder, "prop2");

  b.call<void>("loadString", "a x PropHolder prop=1 prop2=2");
  b.call<void>("loadObjects", false);
  EXPECT_EQ(1, prop.get().toInt());
  EXPECT_EQ(2, prop2.get().toInt());
  b.call<void>("unloadObjects");

  b.call<void>("loadString", "a x PropHolder prop=[1,2] prop2=\"foo\"");
  b.call<void>("loadObjects", false);
  std::vector<int> vi = boost::assign::list_of(1)(2);
  EXPECT_EQ(vi, prop.get().to<std::vector<int> >());
  EXPECT_EQ("foo", prop2.get().toString());
  b.call<void>("unloadObjects");

  prop.set(qi::AnyValue::from(0)); prop2.set(qi::AnyValue::from(0));

  b.call<void>("loadString", "a x PropHolder prop= [ 1 , 2 ]  prop2= \"foo\"  ");
  b.call<void>("loadObjects", false);
  // not c++11 compatible, but this line is not mandatory
  //vi = boost::assign::list_of(1)(2);
  EXPECT_EQ(vi, prop.get().to<std::vector<int> >());
  EXPECT_EQ("foo", prop2.get().toString());
  b.call<void>("unloadObjects");
}

TEST(Behavior, Filter)
{
  TestSessionPair p;
  p.server()->loadService("qicore.Behavior");
  qi::AnyObject b = p.client()->service("Behavior");
  //b.call<void>("connect", p.serviceDirectoryEndpoints()[0].str());
  p.server()->loadService("testbehav.TestObject2");
  p.server()->loadService("testbehav.PropHolder");
  qi::AnyObject propHolder = p.client()->service("PropHolder");
  qi::AnyObject testObject = p.client()->service("TestObject2");
  qi::ProxyProperty<qi::AnyValue> prop(propHolder, "prop");
  qi::ProxyProperty<int> propInt(propHolder, "propInt");
  std::string behavior = STRING(
    to Whatever s.TestObject2 v=0;
    ph Whatever s.PropHolder;
    t1 ph.propInt -> to.add 42;
    //t2 ph.prop -> to.add 51;
    );
  size_t pos = 0;
  while((pos = behavior.find_first_of(';', pos)) != behavior.npos)
    behavior[pos] = '\n';
  std::cerr << "INPUT: " << behavior << std::endl;
  b.call<void>("loadString", behavior);
  b.call<void>("loadObjects", false);
  b.call<void>("setTransitions", false, qi::MetaCallType_Auto);
  propInt.set(12);
  qi::os::msleep(100);
  EXPECT_EQ(0, testObject.call<int>("lastAdd"));
  propInt.set(42);
  qi::os::msleep(100);
  EXPECT_EQ(42, testObject.call<int>("lastAdd"));
  /*
  prop.set(qi::AnyValue::from("foo"));
  qi::os::msleep(100);
  EXPECT_EQ(42, testObject->call<int>("lastAdd"));
  prop.set(qi::AnyValue::from(12));
  qi::os::msleep(100);
  EXPECT_EQ(42, testObject->call<int>("lastAdd"));
  prop.set(qi::AnyValue::from(51));
  qi::os::msleep(100);
  EXPECT_EQ(51, testObject->call<int>("lastAdd"));
  */
}

void pushState(std::vector<std::string>& store,
  const std::string& obj, bool state)
{
  store.push_back(obj + " " + (state?"start":"stop"));
}

void pushError(std::vector<std::string>& store,
  const std::string& obj, const std::string& err)
{
  store.push_back(obj + " error " +err);
}


TEST(Behavior, Task)
{
  TestSessionPair p;
  p.server()->loadService("qicore.Behavior");
  qi::AnyObject b = p.client()->service("Behavior");
  p.server()->loadService("testbehav.TestObject2");
  std::string behavior = STRING(
    sq1 W testbehav.SqrtTaskService   ;
    sq2 W testbehav.SqrtTaskService   ;
    sq3 W testbehav.SqrtTaskService   ;
    st  W testbehav.TestObjectService v=0;
    t1 sq1.val -> sq2.start        ;
    t2 sq2.val -> sq3.start        ;
    ts st.onAdd -> sq1.start       ;
    );
  size_t pos = 0;
  while((pos = behavior.find_first_of(';', pos)) != behavior.npos)
    behavior[pos] = '\n';
  std::cerr << "INPUT: " << behavior << std::endl;
  b.call<void>("loadString", behavior);
  b.call<void>("loadObjects", true);
  b.call<void>("setTransitions", true, qi::MetaCallType_Auto);
  std::vector<std::string> store;
  qi::SignalLink l1 = b.connect("onTaskError",   (boost::function<void(const std::string&, const std::string&)>)boost::bind(&pushError, boost::ref(store), _1, _2));
  qi::SignalLink l2 = b.connect("onTaskRunning", (boost::function<void(const std::string&, bool)>)boost::bind(&pushState, boost::ref(store), _1, _2));
  ASSERT_TRUE(l1 != qi::SignalBase::invalidSignalLink);
  ASSERT_TRUE(l2 != qi::SignalBase::invalidSignalLink);
  qi::AnyObject ts = b.call<qi::AnyObject>("object", "st");
  qi::AnyObject sq3 = b.call<qi::AnyObject>("object", "sq3");
  qi::ProxyProperty<double> sq3_val(sq3, "val");
  ASSERT_TRUE(!!ts);
  ts.call<int>("add", 256);
  PERSIST_CHECK(, sq3_val.get() == 2, , 2000);
  ASSERT_EQ(2.0, sq3_val.get());
  EXPECT_EQ(6u, store.size());
  std::sort(store.begin(), store.end());
  EXPECT_EQ("sq1 start;sq1 stop;sq2 start;sq2 stop;sq3 start;sq3 stop",
    boost::algorithm::join(store, ";"));
  store.clear();
  ts.setProperty("v", -17);
  ts.call<int>("add", 0);
  PERSIST_CHECK(, store.size() == 3, , 2000);
  EXPECT_EQ(3u, store.size());
  std::sort(store.begin(), store.end());
  EXPECT_EQ("sq1 error Negative input;sq1 start;sq1 stop",
    boost::algorithm::join(store, ";"));
}

TEST(Behavior, TaskCall)
{
   std::string behavior = STRING(
     SQ  W testbehav.SqrtService              ;
     sq1 W &SQ.sqrt                    ;
     sq2 W &SQ.sqrt                    ;
     st  W testbehav.TestObjectService v=0;
     t1 st.onAdd -> sq1.start          ;
     t2 sq1.result -> sq2.start        ;
     );
  TestSessionPair p;
  p.server()->loadService("qicore.Behavior");
  qi::AnyObject b = p.client()->service("Behavior");
  size_t pos = 0;
  while((pos = behavior.find_first_of(';', pos)) != behavior.npos)
    behavior[pos] = '\n';
  std::cerr << "INPUT: " << behavior << std::endl;
  b.call<void>("loadString", behavior);
  b.call<void>("loadObjects", true);
  b.call<void>("setTransitions", true, qi::MetaCallType_Auto);
  std::vector<std::string> store;
  qi::SignalLink l1 = b.connect("onTaskError",   (boost::function<void(const std::string&, const std::string&)>)boost::bind(&pushError, boost::ref(store), _1, _2));
  qi::SignalLink l2 = b.connect("onTaskRunning", (boost::function<void(const std::string&, bool)>)boost::bind(&pushState, boost::ref(store), _1, _2));
  ASSERT_TRUE(l1 != qi::SignalBase::invalidSignalLink);
  ASSERT_TRUE(l2 != qi::SignalBase::invalidSignalLink);
  qi::AnyObject ts = b.call<qi::AnyObject>("object", "st");
  qi::AnyObject sq2 = b.call<qi::AnyObject>("object", "sq2");
  qi::ProxyProperty<double> sq2_result(sq2, "result");

  ts.call<int>("add", 256);
  PERSIST_CHECK(, sq2_result.get() == 4, , 2000);
  ASSERT_EQ(4.0, sq2_result.get());
  std::sort(store.begin(), store.end());
  EXPECT_EQ("sq1 start;sq1 stop;sq2 start;sq2 stop",
    boost::algorithm::join(store, ";"));

  store.clear();
  ts.setProperty("v", -17);
  ts.call<int>("add", 0);
  PERSIST_CHECK(, store.size() == 3, , 2000);
  std::sort(store.begin(), store.end());
  EXPECT_EQ("sq1 error Negative input;sq1 start;sq1 stop",
    boost::algorithm::join(store, ";"));
}

int main(int argc, char **argv) {
  qi::Application app(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  TestMode::initTestMode(argc, argv);
  return RUN_ALL_TESTS();
}
