/*
**  Copyright (C) 2013 Aldebaran Robotics
**  See COPYING for the license
*/
#include <qicore-compat/behaviorexecuter.hpp>
#include <qi/session.hpp>
#include <qi/application.hpp>
#include <qi/type/objectfactory.hpp>

#include <boost/program_options.hpp>

namespace po = boost::program_options;
typedef std::map<std::string, qi::AnyObject> ObjectMap;

int main(int argc, char **argv)
{
  qi::Application app(argc, argv);
  std::string masterAddress;
  std::string behavior_path;
  bool debug_mode;
  //declare the program options
  po::options_description desc("Usage :\n  qicore-behavior path/to/behavior [masterAddress] [options]\nOptions");
  desc.add_options()
      ("help", "Print this help.")
      ("behavior", "Path to behavior executed")
      ("master-address",
       po::value<std::string>()->default_value(std::string("tcp://127.0.0.1:9559")),
       "The master address")
      ("debug", po::value<bool>()->default_value(false), "Active debug mode");

  po::positional_options_description pos;
  pos.add("behavior", 1);
  pos.add("master-address", 2);

  // parse and store
  po::variables_map vm;
  try
  {
    po::store(po::command_line_parser(argc, argv).
        options(desc).positional(pos).run(), vm);
    po::notify(vm);

    if(vm.count("help"))
    {
      std::cout << desc << std::endl;
      return 0;
    }

    if(vm.count("master-address") == 1 && vm.count("behavior") == 1 && vm.count("debug") == 1)
    {
      masterAddress = vm["master-address"].as<std::string>();
      behavior_path = vm["behavior"].as<std::string>();
      debug_mode = vm["debug"].as<bool>();
    }
    else
    {
      std::cout << desc << std::endl;
      return 0;
    }

  }
  catch(const boost::program_options::error&)
  {
    std::cout << desc << std::endl;
    return 0;
  }

  boost::shared_ptr<qi::Session> session(new qi::Session);
  try
  {
    session->connect(masterAddress);
  }
  catch(qi::FutureUserException e)
  {
    std::cerr << "Connection error : " << e.what() << std::endl;
    return 1;
  }

  qi::os::dlopen("behavior");
  qi::FutureSync<unsigned int> f = session->registerService("BehaviorService", qi::createObject("BehaviorService"));
  {
    qi::compat::BehaviorExecuter behavior(behavior_path, session, debug_mode);
    if(!behavior.load())
      return 2;
    behavior.execute();
  }

  return 0;
}
