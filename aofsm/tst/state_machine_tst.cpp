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

  using DefaultTransition_t =
      aofsm::StateMachine<SimlpeClient>::DefaultTransition;

  const DefaultTransition_t default_transitions[]
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

  ClientStateMachine_t state_machine2(&simple_client, transitions,
                                      default_transitions);

  ClientStateMachine_t state_machine3(&simple_client, transitions,
                                      default_actions);

  ClientStateMachine_t state_machine4(&simple_client, transitions,
                                      default_transitions, default_actions);
}

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

  using DefaultTransition_t = StateMachine::DefaultTransition;

  using DefaultAction_t = StateMachine::DefaultAction;

  static const Transition_t transitions[];

  static const DefaultTransition_t default_transitions[];

  static const DefaultAction_t default_actions[];

  StateMachine state_machine;
};

const SimlpeClient2::Transition_t SimlpeClient2::transitions[]
    // {Source-State        Event                Destination-State
    //  Actions}
    {{kInitState, kStartAEvt, kAState, &SimlpeClient2::DoStartA},
     {kInitState, kStartBEvt, kBState, &SimlpeClient2::DoStartB},
     {kAState, kEndEvt, kFinalState, &SimlpeClient2::DoEndA},
     {kBState,
      kEndEvt,
      kFinalState,
      {
          &SimlpeClient2::DoPreEndB,
          &SimlpeClient2::DoEndB,
      }}};

const SimlpeClient2::DefaultTransition_t SimlpeClient2::default_transitions[]
    // {Source-State        Event                Destination-State
    //  Actions}
    {{kDefaultEvent,
      kFinalState,
      {
          &SimlpeClient2::DefaultTransition1,
          &SimlpeClient2::DefaultTransition2,
      }}};

const SimlpeClient2::DefaultAction_t SimlpeClient2::default_actions[]
    // {Source-State        Event                Destination-State
    //  Actions}
    {{kDefaultEvent,
      {
          &SimlpeClient2::DefaultAction1,
          &SimlpeClient2::DefaultAction2,
      }}};

SimlpeClient2::SimlpeClient2()
    : state_machine(this, transitions, default_transitions, default_actions) {}

TEST(aofsm_StateMachine, trigger) {
  SimlpeClient2 simple_client;
  simple_client.StartA();
}
