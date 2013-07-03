#include <iostream>
#include <qi/application.hpp>
#include <qimessaging/session.hpp>
#include <qitype/anyobject.hpp>
#include <qitype/objectfactory.hpp>

#define STRING(a) std::string(#a)

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

int main(int argc, char *argv[])
{
  qi::Application app(argc, argv);
  std::string url = "tcp://127.0.0.1:9559";
  qi::Session *ses = new qi::Session;
  ses->connect(url);

  ses->loadService("behavior").size();
  qi::AnyObject b = ses->service("BehaviorService").value()->call<qi::AnyObject>("create");
  b->call<void>("connect", url);
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
  //ASSERT_EQ(42, b->call<int>("call", "a", "getv", arguments()));
  b->call<void>("call", "b", "setv", arguments(1));
  b->call<void>("call", "c", "setv", arguments(2));
  b->call<void>("setTransitions", false);
  //ASSERT_EQ(2, b->call<int>("call", "c", "getv", arguments()));
  b->call<void>("call", "a", "setv", arguments(3));
  qi::os::msleep(1000);
  //ASSERT_EQ(6, b->call<int>("call", "c", "lastAdd", arguments()));

  ses->registerService("paf", b);
  app.run();
}

