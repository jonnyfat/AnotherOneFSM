// copyright Yevgen

#include <cstddef>

using std::size_t;

#include "aofsm/src/state_machine_simple.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

// state machine
//
//  Aktionen habe Parameter (Data*)
//
//  [Init]+->(StartAEvt -> [StateA] -> (EndEvt  +-> [Final]
//        |    \DoStartA(Data*))                \DoEndA(Data*))|
//        |                                      |
//        +->(StartBEvt -> [StateB] -> (EndEvt  +
//             \DoStartB(Data*))                \DoEndB(Data*))
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

class SimlpeClient3 {
 public:
  enum State { kInitState, kAState, kBState, kFinalState, kStateCount };
  enum Event { kStartAEvt, kStartBEvt, kEndEvt, kDefaultEvent, kEventCount };

  struct Data {
    char* data{nullptr};
  };

  SimlpeClient3();

  void StartA(Data* data) { state_machine.Trigger(kStartAEvt, data); }

 private:
  void DoStartA(Data*) {}
  void DoStartB(Data*) {}
  void DoEndA(Data*) {}
  void DoPreEndB(Data*) {}
  void DoEndB(Data*) {}
  void DefaultAction1(Data*) {}
  void DefaultAction2(Data*) {}
  void DefaultTransition1(Data*) {}
  void DefaultTransition2(Data*) {}

  using StateMachine = aofsm::StateMachineSimple<SimlpeClient3, 2, Data*>;

  using Transition_t = StateMachine::StateTransitionDef;

  using DefaultTransition_t = StateMachine::StateMachineDefaultTransitionDef;

  using DefaultAction_t = StateMachine::DefaultAction;

  static const Transition_t transitions[];

  static const DefaultTransition_t default_transitions[];

  static const DefaultAction_t default_actions[];

  StateMachine state_machine;
};

const SimlpeClient3::Transition_t SimlpeClient3::transitions[]
    // {Source-State        Event                Destination-State
    //  Actions}
    {{kInitState, kStartAEvt, kAState, &SimlpeClient3::DoStartA},
     {kInitState, kStartBEvt, kBState, &SimlpeClient3::DoStartB},
     {kAState, kEndEvt, kFinalState, &SimlpeClient3::DoEndA},
     {kBState,
      kEndEvt,
      kFinalState,
      {
          &SimlpeClient3::DoPreEndB,
          &SimlpeClient3::DoEndB,
      }}};

const SimlpeClient3::DefaultTransition_t SimlpeClient3::default_transitions[]
    // {Source-State        Event                Destination-State
    //  Actions}
    {{kDefaultEvent,
      kFinalState,
      {
          &SimlpeClient3::DefaultTransition1,
          &SimlpeClient3::DefaultTransition2,
      }}};

const SimlpeClient3::DefaultAction_t SimlpeClient3::default_actions[]
    // {Source-State        Event                Destination-State
    //  Actions}
    {{kDefaultEvent,
      {
          &SimlpeClient3::DefaultAction1,
          &SimlpeClient3::DefaultAction2,
      }}};

SimlpeClient3::SimlpeClient3()
    : state_machine(this, transitions, default_transitions, default_actions) {}

TEST(aofsm_StateMachineActionWithParam, trigger) {
  SimlpeClient3 simple_client;
  SimlpeClient3::Data data;
  simple_client.StartA(&data);
}
