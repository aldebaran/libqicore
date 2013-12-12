/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2013 All Rights Reserved
*/

#include <gtest/gtest.h>
#include <qi/application.hpp>
#include <qicore-compat/behaviorexecuter.hpp>
#include <alcommon/albroker.h>
#include <allauncher/launcher.h>
#include <qitype/anyvalue.hpp>
#include <qitype/objectfactory.hpp>

qi::Session* session;
std::string behaviorWithoutTimeline;
std::string behaviorWithTimeline;
/*
TEST(BehaviorExecuter, ExecuteBehaviorWithoutTimeline)
{
  qi::compat::BehaviorExecuter exec(behaviorWithoutTimeline, *session, false);

  ASSERT_TRUE(exec.load());
  exec.execute();
}

TEST(BehaviorExecuter, ExecuteDebugWithoutTimeline)
{
  qi::compat::BehaviorExecuter exec(behaviorWithoutTimeline, *session, true);

  ASSERT_TRUE(exec.load());
  exec.execute();
}
*//*
TEST(BehaviorExecuter, ExecuteBehavior)
{
  qi::compat::BehaviorExecuter exec(behaviorWithTimeline, *session, false);

  ASSERT_TRUE(exec.load());
  exec.execute();
}*/

TEST(BehaviorExecuter, ExecuteDebug)
{
  //qi::compat::BehaviorExecuter exec(behaviorWithTimeline, *session, true);

  //ASSERT_TRUE(exec.load());
  //exec.execute();
}

int main(int argc, char **argv)
{
//  qi::Application app(argc, argv);
//  ::testing::InitGoogleTest(&argc, argv);
//
//  if(argc < 3)
//  {
//    std::cerr << "Usage: test_behaviorexecuter /path/to/behavior/directory/without/timeline "
//              << "/path/to/behavior/directory/with/timeline"
//              << std::endl;
//
//    return 2;
//  }
//  boost::shared_ptr<AL::ALBroker> broker;
//  try
//  {
//    qi::os::dlopen("behavior");
//    broker = AL::ALBroker::createBroker(
//          "testbroker",      // broker name
//          "127.0.0.1", 9559, // ip, port
//          "", 0,            // parent ip, parent port empty -> standalone broker
//          0,                 // default flags
//          "",                // default path
//          false              // load ALNetwork
//          );
//    AL::Launcher launcher(broker);
//    launcher.loadLibrary("albase");
//    launcher.loadLibrary("alresourcemanager");
//    launcher.loadLibrary("motion");
//    launcher.loadLibrary("audioout");
//    launcher.loadLibrary("robotmodel");
//    launcher.loadLibrary("leds");
//    session = &(broker->session());
//    session->registerService("BehaviorService", qi::createObject("BehaviorService"));
//  }
//  catch(const std::exception &e)
//  {
//    std::cerr << "Failed to create a test broker" << std::endl
//              << "Error was: " << std::endl
//              << e.what() ;
//    return 1;
//  }
//
//  behaviorWithoutTimeline = std::string(argv[1]);
//  behaviorWithTimeline = std::string(argv[2]);
//  return RUN_ALL_TESTS();
}

