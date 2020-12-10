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
//  [Init]+->(StartAEvt -> [A_STATE] -> (EndEvt  +-> [Final]
//        |    \DoStartA(Data*))                \DoEndA(Data*))|
//        |                                      |
//        +->(StartBEvt -> [B_STATE] -> (EndEvt  +
//             \DoStartB(Data*))                \DoEndB(Data*))
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

class SimlpeClient3 {
 public:
  enum State { INITIAL_STATE, A_STATE, B_STATE, FINAL_STATE, kStateCount };
  enum Event { kStartAEvt, kStartBEvt, kEndEvt, kEventCount };

  struct Data {
    char* data{nullptr};
  };

  void StartA(Data* data) { state_machine.Trigger(kStartAEvt, data); }
  void StartB(Data* data) { state_machine.Trigger(kStartBEvt, data); }

 private:
  void DoStartA(Data*) {}
  void DoStartB(Data*) {}
  void DoEndA(Data*) {}
  void DoEndB(Data*) {}

  using StateMachine = aofsm::StateMachineSimple<SimlpeClient3, 2, Data*>;

  using Transition_t = StateMachine::Transition;

  static const Transition_t transitions[];

  StateMachine state_machine{
      this,  // {Source-State        Event               Destination-State
             //  Actions}
      {      // Transitions
       {INITIAL_STATE, kStartAEvt, A_STATE, &DoStartA},
       {INITIAL_STATE, kStartBEvt, B_STATE, &DoStartB},
       {A_STATE, kEndEvt, FINAL_STATE, &DoEndA},
       {B_STATE, kEndEvt, FINAL_STATE, &DoEndB}}};
};

TEST(aofsm_StateMachineActionWithParam, trigger) {
  SimlpeClient3 simple_client;
  SimlpeClient3::Data data;
  simple_client.StartA(&data);
}
