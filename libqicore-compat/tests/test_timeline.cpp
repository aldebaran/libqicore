/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <qi/qi.hpp>
#include <qi/log.hpp>
#include <gtest/gtest.h>
#include <alcommon/albroker.h>
#include <allauncher/launcher.h>

#include <qicore-compat/timeline.hpp>

std::string file;
boost::shared_ptr<AL::ALBroker> broker;

TEST(QiTimeline, CreateTimeline)
{
  qi::Timeline t(broker);
}

TEST(QiTimeline, OpenFile)
{
  qi::Timeline t(broker);
  t.loadFromFile(file);
}

TEST(QiTimeline, OpenFileAndPlay)
{
  qi::Timeline t(broker);
  t.loadFromFile(file);
  t.play();

  t.waitForTimelineCompletion();
}

TEST(QiTimeline, PlayAndPause)
{
  qi::Timeline t(broker);
  t.loadFromFile(file);
  t.play();
  qi::os::sleep(1);
  t.pause();
  t.play();

  t.waitForTimelineCompletion();
}

TEST(QiTimeline, PlayAndStop)
{
  qi::Timeline t(broker);
  t.loadFromFile(file);
  t.play();
  t.stop();
}

TEST(QiTimeline, modifyFPSBeforeStart)
{
  qi::Timeline t(broker);
  t.loadFromFile(file);

  EXPECT_EQ(t.getFPS(), 25);
  t.setFPS(15);
  EXPECT_EQ(t.getFPS(), 15);
  t.play();

  t.waitForTimelineCompletion();
}

TEST(QiTimeline, modifyFPSAfterStart)
{
  qi::Timeline t(broker);
  t.loadFromFile(file);

  t.play();
  t.setFPS(15);
  EXPECT_EQ(t.getFPS(), 15);

  t.waitForTimelineCompletion();
}

TEST(QiTimeline, goToBeforeStart)
{
  qi::Timeline t(broker);
  t.loadFromFile(file);

  t.goTo(44);
  t.play();

  t.waitForTimelineCompletion();
}

TEST(QiTimeline, goToAfterStart)
{
  qi::Timeline t(broker);
  t.loadFromFile(file);

  t.play();
  t.goTo(44);

  t.waitForTimelineCompletion();
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);

  if (argc < 2)
  {
    std::cerr << "Usage: test_timeline /path/to/timeline/behavior.xar"
              << std::endl;
    return 2;
  }
  file = std::string(argv[1]);

  qi::init(argc, argv);
  qi::log::init(qi::log::info, 1);
  try
  {
    broker = AL::ALBroker::createBroker(
          "testbroker",      // broker name
          "127.0.0.1", 9600, // ip, port
          "", 0,            // parent ip, parent port empty -> standalone broker
          0,                 // default flags
          "",                // default path
          false              // load ALNetwork
          );
    AL::Launcher launcher(broker);
    launcher.loadLibrary("albase");
    launcher.loadLibrary("alresourcemanager");
    launcher.loadLibrary("motion");
  }
  catch(const std::exception &e)
  {
    std::cerr << "Failed to create a test broker" << std::endl
              << "Error was: " << std::endl
              << e.what() ;
    return 1;
  }


  return RUN_ALL_TESTS();
}
