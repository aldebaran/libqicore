#include <iostream>
#include <qi/application.hpp>
#include <qimessaging/session.hpp>
#include <qitype/anyobject.hpp>
#include <qitype/objectfactory.hpp>
#include <qimessaging/applicationsession.hpp>
#include <qicore/behavior.hpp>
#include <qilang/parser.hpp>
#include <qilang/formatter.hpp>
#include <boost/program_options.hpp>

qiLogCategory("qi.behavior");

namespace po = boost::program_options;

QI_TYPE_ENUM_REGISTER(qi::MetaCallType);

#define STRING(a) std::string(#a)

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
  qi::ApplicationSession app(argc, argv);

  po::options_description desc("qilang options");
  desc.add_options()
      ("help,h", "produce help message")
      ("behavior,b", po::value<std::string>()->default_value(""), "the behavior to run")
      ;

  po::positional_options_description p;
  p.add("behavior", 1);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 0;
  }

  // TODO: get filename
  app.start();
  std::string filename = vm["behavior"].as<std::string>();

  qi::SessionPtr ses = app.session();
  //ses->connect(url);

  ses->loadService("behavior").size();
  qi::AnyObject b = ses->service("BehaviorService").value().call<qi::AnyObject>("create");
  b.call<void>("connect", ses->url());

  /*
  std::string behavior = STRING(
    a Whatever TestObjectService.create;
    b Whatever TestObjectService.create;
    c Whatever TestObject2;
    ab a.v -> b.add;
    bc b.onAdd -> c.add;
  );


  std::stringstream ss(behavior);
*/

  qilang::ParseResult pr = qilang::parse(qilang::newFileReader(filename));
  //qilang::ParseResult pr = qilang::parse(qilang::newFileReader(&ss, "<!stream!>"));
  if (pr.hasError()) {
    pr.printMessage(std::cout);
    return 1;
  }
  std::cout << qilang::format(pr.ast) << std::endl;
  qi::BehaviorModel bm = qi::loadBehaviorModel(pr.ast);

  b.call<void>("setModel", bm);

  b.call<void>("loadObjects", true);
  //ASSERT_EQ(42, b.call<int>("call", "a", "getv", arguments()));
  b.call<void>("setTransitions", false, qi::MetaCallType_Auto);
  //ASSERT_EQ(2, b.call<int>("call", "c", "getv", arguments()));
  b.call<void>("call", "a", "setv", arguments(3));
  int res = b.call<int>("call", "c", "lastAdd", arguments());
  std::cout << "last res: " << res << std::endl;
  qi::os::msleep(1000);
  //ASSERT_EQ(6, b.call<int>("call", "c", "lastAdd", arguments()));

  //  std::string behavior = STRING(
//    a Whatever TestObjectService.create;
//    b Whatever TestObjectService.create;
//    c Whatever TestObject2;
//    ab a.v -> b.add;
//    bc b.onAdd -> c.add;
//    );
//  size_t pos = 0;
//  while((pos = behavior.find_first_of(';', pos)) != behavior.npos)
//    behavior[pos] = '\n';
//  std::cerr << "INPUT: " << behavior << std::endl;
//  b.call<void>("loadString", behavior);
//  b.call<void>("loadObjects", true);
//  b.call<void>("call", "a", "setv", arguments(42));
//  //ASSERT_EQ(42, b->call<int>("call", "a", "getv", arguments()));
//  b.call<void>("call", "b", "setv", arguments(1));
//  b.call<void>("call", "c", "setv", arguments(2));
//  b.call<void>("setTransitions", false, qi::MetaCallType_Auto);
//  //ASSERT_EQ(2, b->call<int>("call", "c", "getv", arguments()));
//  b.call<void>("call", "a", "setv", arguments(3));
//  qi::os::msleep(1000);
  //ASSERT_EQ(6, b->call<int>("call", "c", "lastAdd", arguments()));

  ses->registerService(bm.name, b);
  app.run();
}

