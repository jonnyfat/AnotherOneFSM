// copyright Yevgen

#include <cstddef>

using std::size_t;

#include "aofsm/src/state_machine.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

// state machine
//
//  [Init]+->(StartAEvt -> [StateA] -> (EndEvt  +-> [Final]
//        |    \DoStartA)                \DoEndA)|
//        |                                      |
//        +->(StartBEvt -> [StateB] -> (EndEvt  +
//             \DoStartB)                \DoEndB)
//
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

class SimlpeClient2 {
 public:
  SimlpeClient2();

  void StartA() { state_machine.Trigger(kStartAEvt); }

  void StartB() { state_machine.Trigger(kStartBEvt); }

  void End() { state_machine.Trigger(kEndEvt); }

 private:
  void DoStartA() {}
  void DoStartB() {}
  void DoEndA() {}
  void DoEndB() {}

  enum State { INITIAL_STATE, A_STATE, B_STATE, FINAL_STATE, kStateCount };
  enum Event { kStartAEvt, kStartBEvt, kEndEvt, kEventCount };

  friend class aofsm::StateMachine<SimlpeClient2>;

  using StateMachine = aofsm::StateMachine<SimlpeClient2>;

  using Transition_t = StateMachine::Transition;

  static const Transition_t transitions[];

  StateMachine state_machine;
};

// clang-format off

const SimlpeClient2::Transition_t SimlpeClient2::transitions[]
    // {Source-State        Event                Destination-State
    //  Actions}
    {{ INITIAL_STATE,           kStartAEvt,             A_STATE,
                            &DoStartA
    },
    { INITIAL_STATE,           kStartBEvt,             B_STATE,
                            &DoStartB
    },
    { A_STATE,              kEndEvt,                FINAL_STATE,
                            &DoEndA
    },
    { B_STATE,              kEndEvt,                FINAL_STATE,
                            &DoEndB
    }};

// clang-format on

SimlpeClient2::SimlpeClient2() : state_machine(this, transitions) {}

TEST(aofsm_StateMachine, trigger) {
  SimlpeClient2 simple_client;
  simple_client.StartA();
}
