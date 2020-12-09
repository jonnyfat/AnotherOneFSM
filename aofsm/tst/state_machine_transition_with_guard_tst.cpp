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
//            kInitState, kAState, kBState, kFinalState
//   events :
//            kStartEvt, kEndEvt
//   actions :
//             DoStartA, DoStartB, DoEndA, DoEndB,
//   transitions:
//         kInitState -[IsA()]-> kAState
//                kStartEvt/DoStartA
//
//         kInitState -[!IsA()]--> kBState
//                kStartEvt/DoStartB
//
//         kAState -> kFinalState
//              kEndEvt/DoEndA
//
//         kBState -> kFinalState
//              kEndEvt/DoEndB
//

class SimlpeClient4 {
 public:
  enum State { kInitState, kAState, kBState, kFinalState, kStateCount };
  enum Event { kStartEvt, kEndEvt, kDefaultEvent, kEventCount };

  SimlpeClient4(bool is_a);

  void Start() { state_machine.Trigger(kStartEvt); }

  void End() { state_machine.Trigger(kEndEvt); }

 private:
  bool IsA() { return is_a_; }
  void DoStartA() {}
  void DoStartB() {}
  void DoEndA() {}
  void DoEndB() {}

  using StateMachine = aofsm::StateMachineSimple<SimlpeClient4, 1>;

  using Transition_t = StateMachine::Transition;

  bool is_a_{false};

  StateMachine state_machine;
};

SimlpeClient4::SimlpeClient4(bool is_a)
    : is_a_{is_a},
      // clang-format off
      state_machine( this,

              // {Source-State        Event               Destination-State
              //                       Guard
              //                                    GuardIsTrue-Destination-State
              //                          GuardIsTrue-Action
              //                                    GuardIsFalse-Destination-State
              //                          GuardIsFalse-Action
              //                       Actions}
              // {Source-State        Event               Destination-State
              //                        Actions}
              {
               // Transitions
               { kInitState,          kStartEvt,
                                         &IsA,
                                                           kAState,
                                          &DoStartA,
                                                           kBState,
                                          &DoStartB
               },

               { kAState,              kEndEvt,            kFinalState,
                                       &DoEndA
               },

               { kBState,              kEndEvt,            kFinalState,
                                       &DoEndB
               }
              }

          // clang-format on
      ) {}

TEST(aofsm_StateMachineActionWithGuard, trigger) {
  SimlpeClient4 simple_client_a(true);
  simple_client_a.Start();

  SimlpeClient4 simple_client_b(false);
  simple_client_b.Start();
}
