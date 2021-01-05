// Copyright Yevgen
#ifndef AOFSM_TST_CLIENT_V2_H_
#define AOFSM_TST_CLIENT_V2_H_

#include "aofsm/src/aofsm.h"

// state machine
// clang-format off
//  [INITIAL_STATE]+->(StartAEvt -> [A_STATE] -> (EndEvt  +-> [FINAL_STATE]
//                 |   \DoStartA)                 \DoEndA)|
//                 |                                      |
//                 +->(StartBEvt -> [B_STATE] -> (EndEvt  +
//                      \DoStartB)                \DoEndB)
// clang-format on
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
class Client1 {
 public:
  //  void StartA() { state_machine.Trigger(kStartAEvt); }
  //
  //  void StartB() { state_machine.Trigger(kStartBEvt); }
  //
  //  void End() { state_machine.Trigger(kEndEvt); }

  enum State { INITIAL_STATE, A_STATE, B_STATE, FINAL_STATE, kStateCount };
  enum Event { kStartAEvt, kStartBEvt, kEndEvt, kEventCount };

  void DoStartA() {}
  void DoStartB() {}
  void DoEndA() {}
  void DoEndB() {}

  using StateMachine_t = aofsm::StateMachine<Client1>;

  StateMachine_t state_machine;
};

//----------------------------------------------------------------------------------
//   DECL_TRANS
DECL_TRANS(Client1::StateMachine_t, INITIAL_STATE, kStartAEvt, A_STATE,
           DoStartA)

DECL_TRANS(Client1::StateMachine_t, INITIAL_STATE, kStartBEvt, B_STATE,
           DoStartB)

DECL_TRANS(Client1::StateMachine_t, A_STATE, kEndEvt, FINAL_STATE, DoEndA)

DECL_TRANS(Client1::StateMachine_t, B_STATE, kEndEvt, FINAL_STATE, DoEndB)

#endif  // AOFSM_TST_CLIENT_V2_H_
