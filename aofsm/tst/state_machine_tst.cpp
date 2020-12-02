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
  void DoPreEndB() {}
  void DoEndB() {}
};

using ClientStateMachine_t = aofsm::StateMachine<SimlpeClient>;

using State_t = SimlpeClient::State;

using Event_t = SimlpeClient::Event;

using Transition_t = aofsm::StateMachine<SimlpeClient>::Transition;

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

using Configuration_t = aofsm::StateMachine<SimlpeClient>::Configuration;

Configuration_t configuration{transitions, sizeof_array(transitions)};

TEST(aofsm, instantiate) {
  SimlpeClient simple_client;

  ClientStateMachine_t state_machine(&simple_client, configuration);
}
