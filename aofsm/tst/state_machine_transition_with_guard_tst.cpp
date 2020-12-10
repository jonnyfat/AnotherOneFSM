// copyright Yevgen

#include <cstddef>

using std::size_t;

#include "aofsm/src/state_machine_simple.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

// state machine
//
//  In Zustand Init beim Event StartEvt erfolgt übergang in Anhängigkeit von
//  Guard IsA()
//
//  [Init]+->([IsA()]StartEvt -> [StateA] -> (EndEvt         +-> [Final]
//        |          \DoStartA(Data*))        \DoEndA(Data*))|
//        |                                                  |
//        +->([!IsA()]StartEvt -> [StateB] -> (EndEvt        +
//                    \DoStartB(Data*))        \DoEndB(Data*))
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
  enum Event { kStartEvt, kEndEvt, kDefaultEvent, kEventCount };

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

  using Transition_t = StateMachine::Transition;

  bool is_a_{false};

  StateMachine state_machine{
      this,
      // {Source-State        Event               Destination-State
      //                       Guard
      //                                    GuardIsTrue-Destination-State
      //                          GuardIsTrue-Action
      //                                    GuardIsFalse-Destination-State
      //                          GuardIsFalse-Action
      //                       Actions}
      // {Source-State        Event               Destination-State
      //                        Actions}
      {// Transitions
       {INITIAL_STATE, kStartEvt, &IsA, A_STATE, &DoStartA, B_STATE, &DoStartB},

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
