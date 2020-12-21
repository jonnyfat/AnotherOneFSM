// copyright Yevgen
//
// Einfachte Parametrierung:
// - nur einfachte Transitionen mit einem Action

#include <cstddef>

using std::size_t;

#include "aofsm/src/state_machine.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

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
  void StartA() { state_machine.Trigger(kStartAEvt); }

  void StartB() { state_machine.Trigger(kStartBEvt); }

  void End() { state_machine.Trigger(kEndEvt); }

 private:
  enum State { INITIAL_STATE, A_STATE, B_STATE, FINAL_STATE, kStateCount };
  enum Event { kStartAEvt, kStartBEvt, kEndEvt, kEventCount };

  void DoStartA() {}
  void DoStartB() {}
  void DoEndA() {}
  void DoEndB() {}

  friend class aofsm::StateMachine<SimlpeClient1>;

  using StateMachine_t = aofsm::StateMachine<SimlpeClient1>;
  using StateMachineDescription_t = StateMachine_t::StateMachineDescription_t;

  static StateMachineDescription_t state_machine_description;

  StateMachine_t state_machine{this, state_machine_description};
};

SimlpeClient1::StateMachineDescription_t
    SimlpeClient1::state_machine_description{
        {{INITIAL_STATE, kStartAEvt, A_STATE, &DoStartA},
         {INITIAL_STATE, kStartBEvt, B_STATE, &DoStartB},
         {A_STATE, kEndEvt, FINAL_STATE, &DoEndA},
         {B_STATE, kEndEvt, FINAL_STATE, &DoEndB}}};

TEST(aofsm_StateMachine, trigger) { SimlpeClient1 simple_client; }
