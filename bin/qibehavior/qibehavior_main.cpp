#include <iostream>
#include <qi/application.hpp>
#include <qi/session.hpp>
#include <qi/anyobject.hpp>
#include <qi/anymodule.hpp>
#include <qi/applicationsession.hpp>
#include <qicore/behavior.hpp>
#include <qilang/parser.hpp>
#include <qilang/formatter.hpp>
#include <boost/program_options.hpp>


qiLogCategory("qi.behavior");

namespace po = boost::program_options;

//QI_TYPE_ENUM_REGISTER(qi::MetaCallType);

int main(int argc, char *argv[])
{
  qi::ApplicationSession app(argc, argv);

  po::options_description desc("qilang options");
  desc.add_options()
      ("help,h", "produce help message")
      ("behavior,b", po::value<std::string>()->default_value(""), "the behavior to run")
      ("load,l", po::value<std::vector<std::string> >()->multitoken(), "load libraries");
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

  std::string filename = vm["behavior"].as<std::string>();

  //Connect the Session
  app.start();
  qi::SessionPtr ses = app.session();

  qi::Object<qi::Behavior> b(new qi::Behavior(ses));

  std::vector<std::string> vload;

  vload = vm["load"].as< std::vector<std::string> >();

  for (unsigned i = 0; i < vload.size(); ++i) {
    qiLogInfo() << "loading lib:" << vload.at(i);
    qi::import(vload.at(i));
  }

  qilang::ParseResultPtr pr = qilang::parse(qilang::newFileReader(filename));
  if (pr->hasError()) {
    pr->printMessage(std::cout);
    return 1;
  }
  qi::BehaviorModel bm = qi::loadBehaviorModel(pr->ast);

  b->setModel(bm);

  //activate debug mode
  b->loadObjects(true);
  //activate debug mode
  b->setTransitions(false, qi::MetaCallType_Auto);

  qiLogInfo() << "Registering behavior: " << "_Behavior" + bm.name;
  ses->registerService("_Behavior" + bm.name, b);

  b->start();
  app.run();
}
