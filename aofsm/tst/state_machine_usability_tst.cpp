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
//            kInitState, kAState, kBState, kFinalState
//   events :
//            kStartAEvt, kStartBEvt, kEndEvt
//   actions :
//             DoStartA, DoStartB, DoEndA, DoEndB,
//   transitions:
//         kInitState -> kAState
//            kStartAEvt/DoStartA
//
//         kInitState -> kBState
//            kStartBEvt/DoStartB
//
//         kAState -> kFinalState
//              kEndEvt/DoEndA
//
//         kBState -> kFinalState
//              kEndEvt/DoEndB
//

class SimlpeClient2 {
 public:
  SimlpeClient2();

  void StartA() { state_machine.Trigger(kStartAEvt); }

 private:
  void DoStartA() {}
  void DoStartB() {}
  void DoEndA() {}
  void DoPreEndB() {}
  void DoEndB() {}
  void DefaultAction1() {}
  void DefaultAction2() {}
  void DefaultTransition1() {}
  void DefaultTransition2() {}

  enum State { kInitState, kAState, kBState, kFinalState, kStateCount };
  enum Event { kStartAEvt, kStartBEvt, kEndEvt, kDefaultEvent, kEventCount };

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
    {{                      kDefaultEvent,
                            {&SimlpeClient2::DefaultAction1,
                             &SimlpeClient2::DefaultAction2}
    },
    {                       kDefaultEvent,         kFinalState,
                            {&SimlpeClient2::DefaultTransition1,
                             &SimlpeClient2::DefaultTransition2}
    },
    { kInitState,           kStartAEvt,             kAState,
                            &SimlpeClient2::DoStartA
    },
    { kInitState,           kStartBEvt,             kBState,
                            &SimlpeClient2::DoStartB
    },
    { kAState,              kEndEvt,                kFinalState,
                            &SimlpeClient2::DoEndA
    },
    { kBState,              kEndEvt,                kFinalState,
                            {&SimlpeClient2::DoPreEndB,
                             &SimlpeClient2::DoEndB}
    }};

// clang-format on

SimlpeClient2::SimlpeClient2() : state_machine(this, transitions) {}

TEST(aofsm_StateMachine, trigger) {
  SimlpeClient2 simple_client;
  simple_client.StartA();
}
