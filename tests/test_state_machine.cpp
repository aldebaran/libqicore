/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <gtest/gtest.h>

#include <qicore/state_machine.h>
#include <qicore/state.h>
#include <qicore/abstract_transition.h>

class StateTest : public State
{
  public:
    StateTest()
      : State(),
        enterStatePass (false),
        exitStatePass (false)
    {
    }

    virtual void onEnter()
    {
      enterStatePass = true;
    }

    virtual void onExit()
    {
      exitStatePass = true;
    }

    bool enterStatePass;
    bool exitStatePass;
};

class TransitionTest : public AbstractTransition
{
  public:
    TransitionTest(AbstractState* toState)
      : AbstractTransition (toState)
    {
    }

    virtual ~TransitionTest() {};

    virtual void triggerTransition()
    {
      executeTransition();
    }
};

TEST(LegacyStateMachine, CreateLegacyStateMachine)
{
  StateMachine sm;
}

TEST(LegacyStateMachine, OneState)
{
  StateMachine sm;
  StateTest* s = new StateTest;

  sm.addState(s);
  sm.setFinalState(s);
  sm.setInitialState(s);

  sm.play();
  EXPECT_TRUE(sm.isOnFinalState());

  EXPECT_TRUE(s->enterStatePass);
  EXPECT_TRUE(s->exitStatePass);
}

TEST(LegacyStateMachine, TwoStates)
{
  StateMachine sm;
  StateTest* s1 = new StateTest;
  StateTest* s2 = new StateTest;
  TransitionTest* toS2 = new TransitionTest(s2);

  s1->addTransition(toS2);
  sm.addState(s1);
  sm.addState(s2);

  sm.setInitialState(s1);
  sm.setFinalState(s2);

  sm.play();
  toS2->triggerTransition();

  EXPECT_TRUE(sm.isOnFinalState());

  EXPECT_TRUE(s1->enterStatePass);
  EXPECT_TRUE(s1->exitStatePass);

  EXPECT_TRUE(s2->enterStatePass);
  EXPECT_TRUE(s2->exitStatePass);
}

TEST(LegacyStateMachine, TwentyLinearStates)
{
  StateMachine sm;
  StateTest* s[20];
  TransitionTest* toSN[19];

  for (unsigned int i = 0; i < 20; i++)
  {
    s[i] = new StateTest;
    if (i != 0)
      toSN[i - 1] = new TransitionTest(s[i]);
    sm.addState(s[i]);
  }

  for (unsigned int i = 0; i < 19; i++)
  {
    s[i]->addTransition(toSN[i]);
  }

  sm.setInitialState(s[0]);
  sm.setFinalState(s[19]);

  sm.play();

  for (unsigned int i = 0; i < 19; i++)
  {
    toSN[i]->triggerTransition();
  }

  EXPECT_TRUE(sm.isOnFinalState());

  EXPECT_TRUE(s[19]->enterStatePass);
  EXPECT_TRUE(s[19]->exitStatePass);
}

TEST(LegacyStateMachine, FinalInTheMiddle)
{
  StateMachine sm;
  StateTest* s1 = new StateTest;
  StateTest* s2 = new StateTest;
  StateTest* s3 = new StateTest;
  TransitionTest* toS2 = new TransitionTest(s2);
  TransitionTest* toS3 = new TransitionTest(s3);

  s1->addTransition(toS2);
  s2->addTransition(toS3);

  sm.addState(s1);
  sm.addState(s2);
  sm.addState(s3);

  sm.setInitialState(s1);
  sm.setFinalState(s2);

  sm.play();
  toS2->triggerTransition();

  EXPECT_TRUE(sm.isOnFinalState());
  EXPECT_FALSE(s3->enterStatePass);
  EXPECT_FALSE(s3->exitStatePass);
}

TEST(LegacyStateMachine, RemoveFinal)
{
  StateMachine sm;
  StateTest* s1 = new StateTest;
  StateTest* s2 = new StateTest;
  StateTest* s3 = new StateTest;
  TransitionTest* toS2 = new TransitionTest(s2);
  TransitionTest* toS3 = new TransitionTest(s3);

  s1->addTransition(toS2);
  s2->addTransition(toS3);

  sm.addState(s1);
  sm.addState(s2);
  sm.addState(s3);

  EXPECT_TRUE(sm.setInitialState(s1));
  EXPECT_TRUE(sm.setFinalState(s2));
  EXPECT_TRUE(sm.setFinalState(s3));
  EXPECT_TRUE(sm.removeFinalState(s2));

  sm.play();
  toS2->triggerTransition();
  toS3->triggerTransition();

  EXPECT_TRUE(sm.isOnFinalState());
  EXPECT_TRUE(s3->enterStatePass);
  EXPECT_TRUE(s3->exitStatePass);
}

TEST(LegacyStateMachine, ChangeInitial)
{
  StateMachine sm;
  StateTest* s1 = new StateTest;
  StateTest* s2 = new StateTest;
  StateTest* s3 = new StateTest;
  TransitionTest* toS2 = new TransitionTest(s2);
  TransitionTest* toS3 = new TransitionTest(s3);

  s1->addTransition(toS2);
  s2->addTransition(toS3);

  sm.addState(s1);
  sm.addState(s2);
  sm.addState(s3);

  EXPECT_TRUE(sm.setInitialState(s1));
  EXPECT_TRUE(sm.setInitialState(s2));
  EXPECT_TRUE(sm.setFinalState(s3));

  sm.play();

  EXPECT_FALSE(s1->enterStatePass);
  EXPECT_FALSE(s1->exitStatePass);
}

TEST(LegacyStateMachine, ChangeStatesAfterStart)
{
  StateMachine sm;
  StateTest* s1 = new StateTest;
  StateTest* s2 = new StateTest;
  StateTest* s3 = new StateTest;
  TransitionTest* toS2 = new TransitionTest(s2);
  TransitionTest* toS3 = new TransitionTest(s3);

  s1->addTransition(toS2);
  s2->addTransition(toS3);

  sm.addState(s1);
  sm.addState(s2);
  sm.addState(s3);

  EXPECT_TRUE(sm.setInitialState(s1));
  EXPECT_TRUE(sm.setFinalState(s3));

  sm.play();
  toS2->triggerTransition();
  EXPECT_TRUE(sm.setFinalState(s2));

  EXPECT_TRUE(sm.isOnFinalState());
}
