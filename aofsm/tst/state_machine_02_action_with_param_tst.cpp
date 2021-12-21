// copyright Yevgen
//
// Actions bei Transitionen haben Parameter

#include "aofsm/src/std_types.h"

#include "aofsm/src/state_machine.h"
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

class SimlpeClient2 {
 public:
  enum State { INITIAL_STATE, A_STATE, B_STATE, FINAL_STATE, kStateCount };
  enum Event { kStartAEvt, kStartBEvt, kEndEvt, kEventCount };

  struct Data {
    char* data{nullptr};
  };

  void StartA(Data* data) { state_machine_.Trigger(kStartAEvt, data); }
  void StartB(Data* data) { state_machine_.Trigger(kStartBEvt, data); }
  void End() { state_machine_.Trigger(kEndEvt, nullptr); }

 private:
  void DoStartA(Data*) {}
  void DoStartB(Data*) {}
  void DoEndA(Data*) {}
  void DoEndB(Data*) {}

  DECL_STATE_MACHINE_WITH_MULT_ACTIONS_AND_TYPES(SimlpeClient2, state_machine_,
                                                 2, Data*);
};

DEF_STATE_MACHINE(SimlpeClient2, state_machine_){
    {// Transitions
     // {Source-State Event Destination-State Actions}
     {INITIAL_STATE, kStartAEvt, A_STATE, &DoStartA},
     {INITIAL_STATE, kStartBEvt, B_STATE, &DoStartB},
     {A_STATE, kEndEvt, FINAL_STATE, &DoEndA},
     {B_STATE, kEndEvt, FINAL_STATE, &DoEndB}}};

TEST(aofsm_StateMachineActionWithParam, trigger) {
  SimlpeClient2 simple_client;
  SimlpeClient2::Data data;
  simple_client.StartA(&data);
}
