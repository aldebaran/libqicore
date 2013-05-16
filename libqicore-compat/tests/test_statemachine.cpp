/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <gtest/gtest.h>

#include <qi/os.hpp>

#include <qicore-compat/statemachine.hpp>
#include <qicore-compat/box.hpp>
#include <qicore-compat/transition.hpp>

TEST(QiStateMachine, CreateQiStateMachine)
{
  qi::StateMachine sm;
}

TEST(QiStateMachine, OneState)
{
  qi::StateMachine sm;
  qi::Box* s = new qi::Box();

  sm.addState(s);
  sm.setFinalState(s);
  sm.setInitialState(s);

  sm.run();
  EXPECT_TRUE(sm.isOnFinalState());

  sm.stop();

  delete s;
}

TEST(QiStateMachine, TwoStates)
{
  qi::StateMachine sm;
  qi::Box* s1 = new qi::Box();
  qi::Box* s2 = new qi::Box();
  qi::Transition* toS2 = new qi::Transition(s2);

  s1->addTransition(toS2);
  sm.addState(s1);
  sm.addState(s2);

  sm.setInitialState(s1);
  sm.setFinalState(s2);

  sm.run();
  toS2->trigger();

  int timeOut = 42;
  while (timeOut)
  {
    qi::os::msleep(1);
    timeOut--;
    if (sm.getCurrentState() == s2)
      break;
  }

  EXPECT_TRUE(sm.isOnFinalState());
  EXPECT_EQ(sm.getCurrentState(), s2);

  sm.stop();

  delete toS2;
  delete s1;
  delete s2;
}

TEST(QiStateMachine, TwentyLinearStates)
{
  qi::StateMachine sm;
  qi::Box* s[20];
  qi::Transition* toSN[19];

  for (unsigned int i = 0; i < 20; i++)
  {
    s[i] = new qi::Box();
    if (i != 0)
      toSN[i - 1] = new qi::Transition(s[i]);
    sm.addState(s[i]);
  }

  for (unsigned int i = 0; i < 19; i++)
    s[i]->addTransition(toSN[i]);

  sm.setInitialState(s[0]);
  sm.setFinalState(s[19]);

  sm.run();

  for (unsigned int i = 0; i < 20; i++)
  {

    int timeOut = 42;
    while (timeOut)
    {
      qi::os::msleep(1);
      timeOut--;
      if (sm.getCurrentState() == s[i])
        break;
    }

    if (i != 19)
      toSN[i]->trigger();
  }

  EXPECT_TRUE(sm.isOnFinalState());
  EXPECT_EQ(sm.getCurrentState(), s[19]);

  sm.stop();

  for (unsigned int i = 0; i < 20; i++)
  {
    delete s[i];
    if (i != 0)
      delete toSN[i - 1];
  }

}

TEST(QiStateMachine, FinalInTheMiddle)
{
  qi::StateMachine sm;
  qi::Box* s1 = new qi::Box();
  qi::Box* s2 = new qi::Box();
  qi::Box* s3 = new qi::Box();
  qi::Transition* toS2 = new qi::Transition(s2);
  qi::Transition* toS3 = new qi::Transition(s3);

  s1->addTransition(toS2);
  s2->addTransition(toS3);

  sm.addState(s1);
  sm.addState(s2);
  sm.addState(s3);

  sm.setInitialState(s1);
  sm.setFinalState(s2);

  sm.run();
  toS2->trigger();

  int timeOut = 42;
  while (timeOut)
  {
    qi::os::msleep(1);
    timeOut--;
    if (sm.getCurrentState() == s2)
      break;
  }

  EXPECT_TRUE(sm.isOnFinalState());
  EXPECT_EQ(sm.getCurrentState(), s2);

  sm.stop();
}

TEST(QiStateMachine, RemoveFinal)
{
  qi::StateMachine sm;
  qi::Box* s1 = new qi::Box();
  qi::Box* s2 = new qi::Box();
  qi::Box* s3 = new qi::Box();
  qi::Transition* toS2 = new qi::Transition(s2);
  qi::Transition* toS3 = new qi::Transition(s3);

  s1->addTransition(toS2);
  s2->addTransition(toS3);

  sm.addState(s1);
  sm.addState(s2);
  sm.addState(s3);

  EXPECT_TRUE(sm.setInitialState(s1));
  EXPECT_TRUE(sm.setFinalState(s2));
  EXPECT_TRUE(sm.setFinalState(s3));
  EXPECT_TRUE(sm.removeFinalState(s2));

  sm.run();
  toS2->trigger();
  int timeOut = 42;
  while (timeOut)
  {
    qi::os::msleep(1);
    timeOut--;
    if (sm.getCurrentState() == s2)
      break;
  }

  toS3->trigger();
  timeOut = 42;
  while (timeOut)
  {
    qi::os::msleep(1);
    timeOut--;
    if (sm.getCurrentState() == s3)
      break;
  }

  EXPECT_TRUE(sm.isOnFinalState());
  EXPECT_EQ(sm.getCurrentState(), s3);

  sm.stop();

  delete toS2;
  delete toS3;
  delete s1;
  delete s2;
  delete s3;
}

TEST(QiStateMachine, ChangeInitial)
{
  qi::StateMachine sm;
  qi::Box* s1 = new qi::Box();
  qi::Box* s2 = new qi::Box();
  qi::Box* s3 = new qi::Box();
  qi::Transition* toS2 = new qi::Transition(s2);
  qi::Transition* toS3 = new qi::Transition(s3);

  s1->addTransition(toS2);
  s2->addTransition(toS3);

  sm.addState(s1);
  sm.addState(s2);
  sm.addState(s3);

  EXPECT_TRUE(sm.setInitialState(s1));
  EXPECT_TRUE(sm.setInitialState(s2));
  EXPECT_TRUE(sm.setFinalState(s3));

  sm.run();

  EXPECT_TRUE(sm.setFinalState(s3));
  EXPECT_EQ(sm.getCurrentState(), s2);

  sm.stop();

  delete toS2;
  delete toS3;
  delete s1;
  delete s2;
  delete s3;
}

TEST(QiStateMachine, ChangeStatesAfterStart)
{
  qi::StateMachine sm;
  qi::Box* s1 = new qi::Box();
  qi::Box* s2 = new qi::Box();
  qi::Box* s3 = new qi::Box();
  qi::Transition* toS2 = new qi::Transition(s2);
  qi::Transition* toS3 = new qi::Transition(s3);

  s1->addTransition(toS2);
  s2->addTransition(toS3);

  sm.addState(s1);
  sm.addState(s2);
  sm.addState(s3);

  EXPECT_TRUE(sm.setInitialState(s1));
  EXPECT_TRUE(sm.setFinalState(s3));

  sm.run();

  toS2->trigger();

  int timeOut = 42;
  while (timeOut)
  {
    qi::os::msleep(1);
    timeOut--;
    if (sm.getCurrentState() == s3)
      break;
  }

  EXPECT_TRUE(sm.setFinalState(s2));
  EXPECT_TRUE(sm.isOnFinalState());

  sm.stop();

  delete toS2;
  delete toS3;
  delete s1;
  delete s2;
  delete s3;
}
