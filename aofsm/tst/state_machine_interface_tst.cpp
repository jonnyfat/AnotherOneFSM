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
class SimlpeClient {
 public:
  enum State { kInitState, kAState, kBState, kFinalState, kStateCount };
  enum Event { kStartAEvt, kStartBEvt, kEndEvt, kDefaultEvent, kEventCount };

  void DoStartA() {}
  void DoStartB() {}
  void DoEndA() {}
  void DoPreEndB() {}
  void DoEndB() {}
  void DefaultAction1() {}
  void DefaultAction2() {}
  void DefaultTransition1() {}
  void DefaultTransition2() {}
};

using ClientStateMachine_t = aofsm::StateMachine<SimlpeClient>;

using State_t = SimlpeClient::State;

using Event_t = SimlpeClient::Event;

using Transition_t = aofsm::StateMachine<SimlpeClient>::Transition;

TEST(aofsm_StateMachine, instantiate) {
  SimlpeClient simple_client;

  const Transition_t transitions[]
      // {Source-State        Event                Destination-State
      //  Actions}
      {{State_t::kInitState, Event_t::kStartAEvt, State_t::kAState,
        &SimlpeClient::DoStartA},
       {State_t::kInitState, Event_t::kStartBEvt, State_t::kBState,
        &SimlpeClient::DoStartB},
       {State_t::kAState, Event_t::kEndEvt, State_t::kFinalState,
        &SimlpeClient::DoEndA},
       {State_t::kBState,
        Event_t::kEndEvt,
        State_t::kFinalState,
        {
            &SimlpeClient::DoPreEndB,
            &SimlpeClient::DoEndB,
        }}};

  using StateMachineDefaultTransitionDef_t =
      aofsm::StateMachine<SimlpeClient>::StateMachineDefaultTransitionDef;

  const StateMachineDefaultTransitionDef_t default_transitions[]
      // {Source-State        Event                Destination-State
      //  Actions}
      {{Event_t::kDefaultEvent,
        State_t::kFinalState,
        {
            &SimlpeClient::DefaultTransition1,
            &SimlpeClient::DefaultTransition2,
        }}};

  using DefaultAction_t = aofsm::StateMachine<SimlpeClient>::DefaultAction;

  const DefaultAction_t default_actions[]
      // {Source-State        Event                Destination-State
      //  Actions}
      {{Event_t::kDefaultEvent,
        {
            &SimlpeClient::DefaultAction1,
            &SimlpeClient::DefaultAction2,
        }}};

  ClientStateMachine_t state_machine1(&simple_client, transitions);
}
