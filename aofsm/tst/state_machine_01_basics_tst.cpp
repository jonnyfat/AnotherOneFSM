// copyright Yevgen
//
// Einfachte Parametrierung:
// - nur einfachte Transitionen mit einem Action

#include "aofsm/src/std_types.h"

#include "gtest/gtest.h"

#include "aofsm/src/state_machine_v1/state_machine.h"

// state machine
// clang-format off
//  [INITIAL_STATE]+->(StartAEvt -> [A_STATE] -> (EndEvt  +-> [FINAL_STATE]
//                 |   \DoStartA)                 \DoEndA)|
//                 |                                      |
//                 +->(StartBEvt -> [B_STATE] -> (EndEvt  +
//                      \DoStartB)                \DoEndB)
// clang-format on
//   states :
//            INITIAL_STATE, A_STATE, B_STATE, FINAL_STATE
//   events :
//            kStartAEvt, kStartBEvt, kEndEvt
//   actions :
//             DoStartA, DoStartB, DoEndA, DoEndB,
//   transitions:
//         INITIAL_STATE -> A_STATE
//            kStartAEvt/DoStartA
//
//         INITIAL_STATE -> B_STATE
//            kStartBEvt/DoStartB
//
//         A_STATE -> FINAL_STATE
//              kEndEvt/DoEndA
//
//         B_STATE -> FINAL_STATE
//              kEndEvt/DoEndB
//

class SimlpeClient1 {
 public:
  void StartA() { state_machine_.Trigger(kStartAEvt); }

  void StartB() { state_machine_.Trigger(kStartBEvt); }

  void End() { state_machine_.Trigger(kEndEvt); }

  enum State { INITIAL_STATE, A_STATE, B_STATE, FINAL_STATE, kStateCount };
  enum Event { kStartAEvt, kStartBEvt, kEndEvt, kEventCount };

  void DoStartA() {}
  void DoStartB() {}
  void DoEndA() {}
  void DoEndB() {}

  DECL_STATE_MACHINE(SimlpeClient1, state_machine_);
};

DEF_STATE_MACHINE(SimlpeClient1, state_machine_){
    {{INITIAL_STATE, kStartAEvt, A_STATE, &DoStartA},
     {INITIAL_STATE, kStartBEvt, B_STATE, &DoStartB},
     {A_STATE, kEndEvt, FINAL_STATE, &DoEndA},
     {B_STATE, kEndEvt, FINAL_STATE, &DoEndB}}};

TEST(aofsm_StateMachine, MustHaveInitialStateAfterInstantiation) {
  // Act
  SimlpeClient1 simple_client;

  // Assert
  EXPECT_EQ(simple_client.state_machine_.GetCurrentState(),
            SimlpeClient1::State::INITIAL_STATE);
}

TEST(aofsm_StateMachine, MustChangeStateAfterTrigger) {
  // Arrange
  SimlpeClient1 simple_client;

  // Act
  simple_client.state_machine_.Trigger(SimlpeClient1::Event::kStartAEvt);

  // Assert
  EXPECT_EQ(simple_client.state_machine_.GetCurrentState(),
            SimlpeClient1::State::A_STATE);
}

TEST(aofsm_StateMachine, MustChangeOtherStateAfterOtherTrigger) {
  // Arrange
  SimlpeClient1 simple_client;

  // Act
  simple_client.state_machine_.Trigger(SimlpeClient1::Event::kStartBEvt);

  // Assert
  EXPECT_EQ(simple_client.state_machine_.GetCurrentState(),
            SimlpeClient1::State::B_STATE);
}
