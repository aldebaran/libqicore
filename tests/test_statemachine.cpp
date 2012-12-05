/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <gtest/gtest.h>

#include <qi/os.hpp>

#include <qicore/statemachine.hpp>
#include <qicore/box.hpp>
#include <qicore/transition.hpp>

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

  EXPECT_TRUE(sm.isOnFinalState());
  EXPECT_EQ(sm.getCurrentState(), s2);

  sm.stop();
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

  for (unsigned int i = 0; i < 19; i++)
    toSN[i]->trigger();

  EXPECT_TRUE(sm.isOnFinalState());
  EXPECT_EQ(sm.getCurrentState(), s[19]);

  sm.stop();
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
  toS3->trigger();

  EXPECT_TRUE(sm.isOnFinalState());
  EXPECT_EQ(sm.getCurrentState(), s3);

  sm.stop();
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
  EXPECT_TRUE(sm.setFinalState(s2));
  EXPECT_TRUE(sm.isOnFinalState());

  sm.stop();
}

TEST(QiStateMachine, TwoStatesWithTimeOut)
{
  qi::StateMachine sm;
  qi::Box* s1 = new qi::Box();
  qi::Box* s2 = new qi::Box();
  qi::Transition* toS2 = new qi::Transition(s2);

  s1->addTransition(toS2);
  toS2->setTimeOut(300);
  sm.addState(s1);
  sm.addState(s2);

  sm.setInitialState(s1);
  sm.setFinalState(s2);

  sm.run();
  qi::os::sleep(1);

  EXPECT_TRUE(sm.isOnFinalState());
  EXPECT_EQ(sm.getCurrentState(), s2);

  sm.stop();
}

TEST(QiStateMachine, TenLinearStatesWithTimeOut)
{
  qi::StateMachine sm;
  qi::Box* s[10];
  qi::Transition* toSN[9];

  for (unsigned int i = 0; i < 10; i++)
  {
    s[i] = new qi::Box();
    if (i != 0)
    {
      toSN[i - 1] = new qi::Transition(s[i]);
      toSN[i - 1]->setTimeOut(50);
    }
    sm.addState(s[i]);
  }

  for (unsigned int i = 0; i < 9; i++)
    s[i]->addTransition(toSN[i]);

  sm.setInitialState(s[0]);
  sm.setFinalState(s[9]);

  sm.run();
  qi::os::sleep(2);

  EXPECT_TRUE(sm.isOnFinalState());
  EXPECT_EQ(sm.getCurrentState(), s[9]);

  sm.stop();
}

TEST(QiStateMachine, FourStatesMixedTimeOut)
{
  qi::StateMachine sm;
  qi::Box* s1 = new qi::Box();
  qi::Box* s2 = new qi::Box();
  qi::Box* s3 = new qi::Box();
  qi::Box* s4 = new qi::Box();
  qi::Transition* toS2 = new qi::Transition(s2);
  qi::Transition* toS3 = new qi::Transition(s3);
  qi::Transition* toS4 = new qi::Transition(s4);

  s1->addTransition(toS2);
  s2->addTransition(toS3);
  s3->addTransition(toS4);
  toS2->setTimeOut(100);
  toS4->setTimeOut(100);
  sm.addState(s1);
  sm.addState(s2);
  sm.addState(s3);
  sm.addState(s4);

  sm.setInitialState(s1);
  sm.setFinalState(s4);

  sm.run();
  qi::os::sleep(1);
  toS3->trigger();
  qi::os::sleep(1);

  EXPECT_TRUE(sm.isOnFinalState());
  EXPECT_EQ(sm.getCurrentState(), s4);

  sm.stop();
}
