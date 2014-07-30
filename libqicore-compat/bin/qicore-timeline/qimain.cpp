/*
**  Copyright (C) 2012 Aldebaran Robotics
**  See COPYING for the license
*/

#include <qicore-compat/model/animationmodel.hpp>
#include <qicore-compat/timeline.hpp>

#include <qi/session.hpp>
#include <qi/application.hpp>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char **argv)
{
  qi::Application a(argc, argv);
  std::string masterAddress;
  std::string animation_path;
  //declare the program options
  po::options_description desc("Usage :\n  qicore-timeline path/to/animation.anim [masterAddress] [options]\nOptions");
  desc.add_options()
      ("help", "Print this help.")
      ("animation", "Path to animation executed")
      ("master-address",
       po::value<std::string>()->default_value(std::string("tcp://127.0.0.1:9559")),
       "The master address");

  po::positional_options_description pos;
  pos.add("animation", 1);
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

    if(vm.count("master-address") == 1 && vm.count("animation") == 1)
    {
      masterAddress = vm["master-address"].as<std::string>();
      animation_path = vm["animation"].as<std::string>();
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

  qi::Session session;
  try
  {
    session.connect(masterAddress);
  }
  catch(qi::FutureUserException e)
  {
    std::cerr << "Connection error : " << e.what() << std::endl;
    return 1;
  }

  qi::AnyObject memory;
  qi::AnyObject motion;
  try
  {
    memory = session.service("ALMemory");
    motion = session.service("ALMotion");
  }
  catch(qi::FutureUserException e)
  {
    std::cerr << e.what() <<std::endl;
    session.close();
    return 1;
  }


  qi::Timeline timeline(motion);
  qi::AnimationModel*anim = new qi::AnimationModel(animation_path);

  if(!anim->loadFromFile())
  {
    session.close();
    return 0;
  }

  timeline.setAnimation(anim);
  timeline.play();
  timeline.waitForTimelineCompletion();
  delete anim;
  session.close();

  return 0;
}
