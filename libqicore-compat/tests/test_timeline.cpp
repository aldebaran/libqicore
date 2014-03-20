/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <qi/qi.hpp>
#include <qi/log.hpp>
#include <gtest/gtest.h>
#include <alcommon/albroker.h>
#include <allauncher/launcher.h>

#include <qi/application.hpp>
#include <qicore-compat/timeline.hpp>
#include <qicore-compat/model/animationmodel.hpp>

std::string file;
qi::AnyObject motion;
qi::AnyObject memory;

TEST(QiTimeline, CreateTimeline)
{
  qi::Timeline t(motion);
}

TEST(QiTimeline, OpenFile)
{
  qi::Timeline t(motion);
  qi::AnimationModel* anim = new qi::AnimationModel(file);
  anim->loadFromFile();
  t.setAnimation(anim);
  delete anim;
}

TEST(QiTimeline, OpenFileAndPlay)
{
  qi::Timeline t(motion);
  qi::AnimationModel* anim = new qi::AnimationModel(file);
  anim->loadFromFile();
  t.setAnimation(anim);
  t.play();

  t.waitForTimelineCompletion();
  delete anim;
}

TEST(QiTimeline, PlayAndPause)
{
  qi::Timeline t(motion);
  //t.loadFromFile(file);
  qi::AnimationModel* anim = new qi::AnimationModel(file);
  anim->loadFromFile();
  t.setAnimation(anim);
  t.play();
  qi::os::sleep(1);
  t.pause();
  t.play();

  t.waitForTimelineCompletion();
  delete anim;
}

TEST(QiTimeline, PlayAndStop)
{
  qi::Timeline t(motion);
  //t.loadFromFile(file);
  qi::AnimationModel* anim = new qi::AnimationModel(file);
  anim->loadFromFile();
  t.setAnimation(anim);
  t.play();
  t.stop();
  delete anim;
}

TEST(QiTimeline, modifyFPSBeforeStart)
{
  qi::Timeline t(motion);
  //t.loadFromFile(file);
  qi::AnimationModel* anim = new qi::AnimationModel(file);
  anim->loadFromFile();
  t.setAnimation(anim);

  EXPECT_EQ(t.getFPS(), 25);
  t.setFPS(15);
  EXPECT_EQ(t.getFPS(), 15);
  t.play();

  t.waitForTimelineCompletion();
  delete anim;
}

TEST(QiTimeline, modifyFPSAfterStart)
{
  qi::Timeline t(motion);
  //t.loadFromFile(file);
  qi::AnimationModel* anim = new qi::AnimationModel(file);
  anim->loadFromFile();
  t.setAnimation(anim);

  t.play();
  t.setFPS(15);
  EXPECT_EQ(t.getFPS(), 15);

  t.waitForTimelineCompletion();
  delete anim;
}

TEST(QiTimeline, goToBeforeStart)
{
  qi::Timeline t(motion);
  //t.loadFromFile(file);
  qi::AnimationModel* anim = new qi::AnimationModel(file);
  anim->loadFromFile();
  t.setAnimation(anim);

  t.goTo(44);
  t.play();

  t.waitForTimelineCompletion();
  delete anim;
}

TEST(QiTimeline, goToAfterStart)
{
  qi::Timeline t(motion);
  //t.loadFromFile(file);
  qi::AnimationModel* anim = new qi::AnimationModel (file);
  anim->loadFromFile();
  t.setAnimation(anim);

  t.play();
  t.goTo(44);

  t.waitForTimelineCompletion();
  delete anim;
}

void noop_deleter(qi::Session*)
{
}

int main(int argc, char** argv)
{
  qi::Application app(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);

  if (argc < 2)
  {
    std::cerr << "Usage: test_timeline /path/to/timeline/behavior.xar"
              << std::endl;
    return 2;
  }
  file = std::string(argv[1]);
  boost::shared_ptr<AL::ALBroker> broker;
  qi::Session lsession;
  try
  {
    qi::os::dlopen("behavior");
    lsession.connect(qi::Url("tcp://127.0.0.1:9559"));
    memory = lsession.service("ALMemory");
    motion = lsession.service("ALMotion");
  }
  catch(const std::exception &e)
  {
    std::cerr << "Failed to connect session" << std::endl
              << "Error was: " << std::endl
              << e.what() ;
    return 1;
  }

  return RUN_ALL_TESTS();
}
