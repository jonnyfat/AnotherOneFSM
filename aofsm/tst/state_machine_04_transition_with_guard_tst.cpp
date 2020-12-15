// copyright Yevgen
//
// Pro Zustand und Event: ein Guard, mit bool-Rückgabewert, und zwei Optionale
// Übergänge (Ziel-Zustand und Actions)
#include <cstddef>

using std::size_t;

#include "aofsm/src/state_machine_simple.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

// state machine
//
//  Im Zustand INIT beim Event StartEvt erfolgt Übergang in Abhängigkeit von
//  Guard IsA().
//
// clang-format off
//
//  [INITIAL]+->([IsA()]StartEvt -> [A_STATE] -> (EndEvt         +-> [FINAL]
//           |          \DoStartA(Data*))         \DoEndA(Data*))|
//           |                                                   |
//           +->([!IsA()]StartEvt -> [B_STATE] -> (EndEvt        +
//                       \DoStartB(Data*))         \DoEndB(Data*))
// clang-format on
//
//   states :
//            INITIAL_STATE, A_STATE, B_STATE, FINAL_STATE
//   events :
//            kStartEvt, kEndEvt
//   actions :
//             DoStartA, DoStartB, DoEndA, DoEndB,
//   transitions:
//         INITIAL_STATE -[IsA()]-> A_STATE
//                kStartEvt/DoStartA
//
//         INITIAL_STATE -[!IsA()]--> B_STATE
//                kStartEvt/DoStartB
//
//         A_STATE -> FINAL_STATE
//              kEndEvt/DoEndA
//
//         B_STATE -> FINAL_STATE
//              kEndEvt/DoEndB
//

class SimlpeClient4 {
 public:
  enum State { INITIAL_STATE, A_STATE, B_STATE, FINAL_STATE, kStateCount };
  enum Event { kStartEvt, kEndEvt, kEventCount };

  SimlpeClient4(bool is_a);

  void Start() { state_machine.Trigger(kStartEvt); }

  void End() { state_machine.Trigger(kEndEvt); }

 private:
  bool IsA() const { return is_a_; }
  void DoStartA() {}
  void DoStartB() {}
  void DoEndA() {}
  void DoEndB() {}

  using StateMachine = aofsm::StateMachineSimple<SimlpeClient4, 1>;

  bool is_a_{false};

  StateMachine state_machine{
      this,
      {//  Conditional-Transitions
       // {Source-State Event Guard  GuardIsTrue-Destination-State
       //                                  GuardIsTrue-Action
       //                            GuardIsFalse-Destination-State
       //                                   GuardIsFalse-Action}
       {INITIAL_STATE, kStartEvt, &IsA, A_STATE, &DoStartA, B_STATE, &DoStartB},
       //
       // Transitions
       // {Source-State Event  Destination-State  Actions}
       {A_STATE, kEndEvt, FINAL_STATE, &DoEndA},
       {B_STATE, kEndEvt, FINAL_STATE, &DoEndB}}};
};

SimlpeClient4::SimlpeClient4(bool is_a) : is_a_{is_a} {}

TEST(aofsm_StateMachineActionWithGuard, trigger) {
  SimlpeClient4 simple_client_a(true);
  simple_client_a.Start();

  SimlpeClient4 simple_client_b(false);
  simple_client_b.Start();
}
