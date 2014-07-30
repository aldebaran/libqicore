/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2013 All Rights Reserved
*/

#include <gtest/gtest.h>
#include <qi/application.hpp>
#include <qicore-compat/behaviorexecuter.hpp>
#include <alcommon/albroker.h>
#include <allauncher/launcher.h>
#include <qi/anyvalue.hpp>
#include <qi/type/objectfactory.hpp>

boost::shared_ptr<qi::Session> session;
std::string behaviorWithoutTimeline;
std::string behaviorWithTimeline;

TEST(BehaviorExecuter, ExecuteBehaviorWithoutTimeline)
{
  qi::compat::BehaviorExecuter exec(behaviorWithoutTimeline, session, false);

  ASSERT_TRUE(exec.load());
  exec.execute();
}

TEST(BehaviorExecuter, ExecuteDebugWithoutTimeline)
{
  qi::compat::BehaviorExecuter exec(behaviorWithoutTimeline, session, true);

  ASSERT_TRUE(exec.load());
  exec.execute();
}

TEST(BehaviorExecuter, ExecuteBehavior)
{
  qi::compat::BehaviorExecuter exec(behaviorWithTimeline, session, false);

  ASSERT_TRUE(exec.load());
  exec.execute();
}

TEST(BehaviorExecuter, ExecuteDebug)
{
  qi::compat::BehaviorExecuter exec(behaviorWithTimeline, session, true);

  ASSERT_TRUE(exec.load());
  exec.execute();
}

void noop_deleter(qi::Session*)
{
}

int main(int argc, char **argv)
{
  qi::Application app(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);

  if(argc < 3)
  {
    std::cerr << "Usage: test_behaviorexecuter /path/to/behavior/directory/without/timeline "
              << "/path/to/behavior/directory/with/timeline"
              << std::endl;

    return 2;
  }
  boost::shared_ptr<AL::ALBroker> broker;
  qi::Session lsession;
  try
  {
    qi::os::dlopen("behavior");
    lsession.connect(qi::Url("tcp://127.0.0.1:9559"));
    lsession.registerService("BehaviorService", qi::createObject("BehaviorService"));
    session = boost::shared_ptr<qi::Session>(&lsession, noop_deleter);
  }
  catch(const std::exception &e)
  {
    std::cerr << "Failed to connect session" << std::endl
              << "Error was: " << std::endl
              << e.what() ;
    return 1;
  }

  behaviorWithoutTimeline = std::string(argv[1]);
  behaviorWithTimeline = std::string(argv[2]);
  return RUN_ALL_TESTS();
}
