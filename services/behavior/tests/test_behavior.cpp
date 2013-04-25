#include <gtest/gtest.h>

#include <qi/application.hpp>

#include <qitype/objectfactory.hpp>

#include <qimessaging/session.hpp>
#include <testsession/testsessionpair.hpp>


std::vector<qi::GenericValue> arguments(qi::AutoGenericValuePtr v1 = qi::AutoGenericValuePtr(),
  qi::AutoGenericValuePtr v2 = qi::AutoGenericValuePtr(),
  qi::AutoGenericValuePtr v3 = qi::AutoGenericValuePtr(),
  qi::AutoGenericValuePtr v4 = qi::AutoGenericValuePtr(),
  qi::AutoGenericValuePtr v5 = qi::AutoGenericValuePtr())
{
  std::vector<qi::GenericValue> res;
  if (v1.value)
    res.push_back(qi::GenericValue(v1));
  if (v2.value)
    res.push_back(qi::GenericValue(v2));
  if (v3.value)
    res.push_back(qi::GenericValue(v3));
  if (v4.value)
    res.push_back(qi::GenericValue(v4));
  if (v5.value)
    res.push_back(qi::GenericValue(v5));
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
  qi::Signal<void(int)> onAdd;
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
  qi::Signal<void(int)> onAdd;
  int lastRes;
};
QI_REGISTER_OBJECT(TestObject2, add, getv, setv, v, onAdd, lastAdd);


// TestObjectService::create
QI_REGISTER_OBJECT_FACTORY_BUILDER(TestObject);
QI_REGISTER_OBJECT_FACTORY_CONSTRUCTOR(TestObject2);

#define STRING(a) std::string(#a)
TEST(Behavior, test)
{
  TestSessionPair p;
  ASSERT_EQ(1u, p.server()->loadService("behavior").size());
  qi::ObjectPtr b = p.client()->service("BehaviorService").value()->call<qi::ObjectPtr>("create");
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
  b->call<void>("setTransitions");
  ASSERT_EQ(2, b->call<int>("call", "c", "getv", arguments()));
  b->call<void>("call", "a", "setv", arguments(3));
  qi::os::msleep(1000);
  ASSERT_EQ(6, b->call<int>("call", "c", "lastAdd", arguments()));
  /* not a service
  qi::ObjectPtr daC = p.client()->service("TestObject2");
  ASSERT_EQ(6, daC->call<int>("lastAdd"));*/
}


int main(int argc, char **argv) {
  qi::Application app(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  TestMode::initTestMode(argc, argv);
  return RUN_ALL_TESTS();
}
