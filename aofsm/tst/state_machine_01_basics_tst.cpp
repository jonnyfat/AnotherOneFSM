// copyright Yevgen
//
// Einfachte Parametrierung:
// - nur einfachte Transitionen mit einem Action

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "aofsm/src/std_types.h"

#include "aofsm/src/state_machine.h"

// state machine
// clang-format off
//  [INITIAL_STATE]+->(StartAEvt -> [A_STATE]
//                 |   \DoStartA)
//                 |                                      |
//                 +->(StartBEvt -> [B_STATE]
//                      \DoStartB)
// clang-format on
//   states :
//            INITIAL_STATE, A_STATE, B_STATE
//   events :
//            kStartAEvt, kStartBEvt
//   actions :
//             DoStartA, DoStartB
//   transitions:
//         INITIAL_STATE -> A_STATE
//            kStartAEvt/DoStartA
//
//         INITIAL_STATE -> B_STATE
//            kStartBEvt/DoStartB
//

class SimlpeClient1 {
 public:
  enum State { INITIAL_STATE, A_STATE, B_STATE, kStateCount };
  enum Event { kStartAEvt, kStartBEvt, kEventCount };

  MOCK_METHOD(void, DoStartA, (), ());
  MOCK_METHOD(void, DoStartB, (), ());

  DECL_STATE_MACHINE(SimlpeClient1, state_machine_);
};

DEF_STATE_MACHINE(SimlpeClient1, state_machine_,
                  {INITIAL_STATE, kStartAEvt, A_STATE, &DoStartA},
                  {INITIAL_STATE, kStartBEvt, B_STATE, &DoStartB});

TEST(aofsm_StateMachine, MustHaveInitialStateAfterInstantiation) {
  // Act
  SimlpeClient1 simple_client;
  EXPECT_CALL(simple_client, DoStartA).Times(0);
  EXPECT_CALL(simple_client, DoStartB).Times(0);

  // Assert
  EXPECT_EQ(simple_client.state_machine_.GetCurrentState(),
            SimlpeClient1::State::INITIAL_STATE);
}

TEST(aofsm_StateMachine, MustChangeStateAfterTrigger) {
  // Arrange
  SimlpeClient1 simple_client;
  EXPECT_CALL(simple_client, DoStartA).Times(1);
  EXPECT_CALL(simple_client, DoStartB).Times(0);

  // Act
  simple_client.state_machine_.Trigger(SimlpeClient1::Event::kStartAEvt);

  // Assert
  EXPECT_EQ(simple_client.state_machine_.GetCurrentState(),
            SimlpeClient1::State::A_STATE);
}

TEST(aofsm_StateMachine, MustChangeOtherStateAfterOtherTrigger) {
  // Arrange
  SimlpeClient1 simple_client;
  EXPECT_CALL(simple_client, DoStartA).Times(0);
  EXPECT_CALL(simple_client, DoStartB).Times(1);

  // Act
  simple_client.state_machine_.Trigger(SimlpeClient1::Event::kStartBEvt);

  // Assert
  EXPECT_EQ(simple_client.state_machine_.GetCurrentState(),
            SimlpeClient1::State::B_STATE);
}
