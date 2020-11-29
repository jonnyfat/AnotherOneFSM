// copyright Yevgen

#include <cstddef>

using std::size_t;

#include "aofsm/src/state_machine.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

template <class T, size_t N>
constexpr size_t sizeof_array(T (&)[N]) {
  return N;
}
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
  enum Event { kStartAEvt, kStartBEvt, kEndEvt, kEventCount };

  void DoStartA() {}
  void DoStartB() {}
  void DoEndA() {}
  void DoEndB() {}
};

using ClientStateMachine_t = aofsm::StateMachine<SimlpeClient>;

using State_t = SimlpeClient::State;

using Event_t = SimlpeClient::Event;

using Transition_t = ClientStateMachine_t::Transition;

Transition_t transitions[] =
    // {Source-State        Event                Action
    //  Destination-State}
    {{State_t::kInitState, Event_t::kStartAEvt, &SimlpeClient::DoStartA,
      State_t::kAState},
     {State_t::kInitState, Event_t::kStartBEvt, &SimlpeClient::DoStartB,
      State_t::kBState},
     {State_t::kAState, Event_t::kEndEvt, &SimlpeClient::DoEndA,
      State_t::kFinalState},
     {State_t::kBState, Event_t::kEndEvt, &SimlpeClient::DoEndB,
      State_t::kFinalState}};

using ClientStateMachineConfig_t = ClientStateMachine_t::Configuration;

ClientStateMachineConfig_t configuration{sizeof_array(transitions),
                                         transitions};

TEST(aofsm, instantiate) {
  SimlpeClient simple_client;

  ClientStateMachine_t state_machine(&simple_client, configuration);
}
