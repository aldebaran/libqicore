/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <gtest/gtest.h>

#include <newframemanager/legacy_state_machine.h>
#include <newframemanager/legacy_state.h>

class StateTest : public LegacyState
{
  public:
    StateTest()
      : LegacyState(),
        enterStatePass (false),
        exitStatePass (false),
        updatePass (false),
        next (0)
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

    virtual AbstractState* update()
    {
      updatePass = true;

      return next;
    }

    bool enterStatePass;
    bool exitStatePass;
    bool updatePass;

    AbstractState* next;
};

TEST(LegacyStateMachine, CreateLegacyStateMachine)
{
  LegacyStateMachine sm;
}

TEST(LegacyStateMachine, OneState)
{
  LegacyStateMachine sm;
  StateTest* s = new StateTest;

  sm.addState(s);
  sm.setFinalState(s);
  sm.setInitialState(s);

  sm.play();
  sm.waitForExecuterCompletion();

  EXPECT_TRUE(s->enterStatePass);
  EXPECT_TRUE(s->exitStatePass);
  EXPECT_TRUE(s->updatePass);
}

TEST(LegacyStateMachine, TwoStates)
{
  LegacyStateMachine sm;
  StateTest* s1 = new StateTest;
  StateTest* s2 = new StateTest;

  sm.addState(s1);
  sm.addState(s2);
  s1->next = s2;

  sm.setInitialState(s1);
  sm.setFinalState(s2);

  sm.play();
  sm.waitForExecuterCompletion();

  EXPECT_TRUE(s1->enterStatePass);
  EXPECT_TRUE(s1->exitStatePass);
  EXPECT_TRUE(s1->updatePass);

  EXPECT_TRUE(s2->enterStatePass);
  EXPECT_TRUE(s2->exitStatePass);
  EXPECT_TRUE(s2->updatePass);
}

TEST(LegacyStateMachine, TwentyLinearStates)
{
  LegacyStateMachine sm;
  StateTest* s[20];

  for (unsigned int i = 0; i < 20; i++)
  {
    s[i] = new StateTest;
    sm.addState(s[i]);
  }

  for (unsigned int i = 0; i < 19; i++)
  {
    s[i]->next = s[i + 1];
  }

  sm.setInitialState(s[0]);
  sm.setFinalState(s[19]);

  sm.play();
  sm.waitForExecuterCompletion();

  EXPECT_TRUE(s[19]->enterStatePass);
  EXPECT_TRUE(s[19]->exitStatePass);
  EXPECT_TRUE(s[19]->updatePass);
}

TEST(LegacyStateMachine, FinalInTheMiddle)
{
  LegacyStateMachine sm;
  StateTest* s1 = new StateTest;
  StateTest* s2 = new StateTest;
  StateTest* s3 = new StateTest;

  sm.addState(s1);
  sm.addState(s2);
  sm.addState(s3);
  s1->next = s2;
  s2->next = s3;

  sm.setInitialState(s1);
  sm.setFinalState(s2);

  sm.play();
  sm.waitForExecuterCompletion();

  EXPECT_TRUE(sm.isOnFinalState());
  EXPECT_FALSE(s3->enterStatePass);
  EXPECT_FALSE(s3->exitStatePass);
  EXPECT_FALSE(s3->updatePass);
}

TEST(LegacyStateMachine, RemoveFinal)
{
  LegacyStateMachine sm;
  StateTest* s1 = new StateTest;
  StateTest* s2 = new StateTest;
  StateTest* s3 = new StateTest;

  sm.addState(s1);
  sm.addState(s2);
  sm.addState(s3);
  s1->next = s2;
  s2->next = s3;

  EXPECT_TRUE(sm.setInitialState(s1));
  EXPECT_TRUE(sm.setFinalState(s2));
  EXPECT_TRUE(sm.setFinalState(s3));
  EXPECT_TRUE(sm.removeFinalState(s2));

  sm.play();
  sm.waitForExecuterCompletion();

  EXPECT_TRUE(s3->enterStatePass);
  EXPECT_TRUE(s3->exitStatePass);
  EXPECT_TRUE(s3->updatePass);
}

TEST(LegacyStateMachine, ChangeInitial)
{
  LegacyStateMachine sm;
  StateTest* s1 = new StateTest;
  StateTest* s2 = new StateTest;
  StateTest* s3 = new StateTest;

  sm.addState(s1);
  sm.addState(s2);
  sm.addState(s3);
  s1->next = s2;
  s2->next = s3;

  EXPECT_TRUE(sm.setInitialState(s1));
  EXPECT_TRUE(sm.setInitialState(s2));
  EXPECT_TRUE(sm.setFinalState(s3));

  sm.play();
  sm.waitForExecuterCompletion();

  EXPECT_FALSE(s1->enterStatePass);
  EXPECT_FALSE(s1->exitStatePass);
  EXPECT_FALSE(s1->updatePass);
}

TEST(LegacyStateMachine, ChangeStatesAfterStart)
{
  LegacyStateMachine sm;
  StateTest* s1 = new StateTest;
  StateTest* s2 = new StateTest;
  StateTest* s3 = new StateTest;

  sm.addState(s1);
  sm.addState(s2);
  sm.addState(s3);
  s1->next = s2;
  s2->next = s3;

  EXPECT_TRUE(sm.setInitialState(s1));
  EXPECT_TRUE(sm.setFinalState(s3));

  sm.play();
  EXPECT_FALSE(sm.setInitialState(s2));
  EXPECT_FALSE(sm.setFinalState(s3));

  sm.waitForExecuterCompletion();
}
