// copyright Yevgen

#include <cstddef>

using std::size_t;

#include "aofsm/src/state_machine.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

// state machine
//
//  [Init]+->(StartEvt -> [State] -> (EndEvt  +-> [Final]
//        |    \DoStart)               \DoTick)|
//        |    \DoTick)                \DoEnd)|
//                     <-ResetEvt
//                       \DoReset
//   states :
//            INITIAL_STATE, kMiddleState, FINAL_STATE
//   events :
//            kStartEvt, kEndEvt, kResetEvt
//   actions :
//             DoStart, DoTick, DoEnd, DoReset,
//   transitions:
//         INITIAL_STATE -> kMiddleState
//            kStartEvt/DoStart,DoTick
//
//         kMiddleState -> FINAL_STATE
//              kEndEvt/DoTick,DoEnd
//
//         FINAL_STATE -> B_STATE
//              kResetEvt/DoReset
//
class SimlpeClient {
 public:
  enum State { INITIAL_STATE, MIDDLE_STATE, FINAL_STATE, kStateCount };
  enum Event { kStartEvt, kEndEvt, kResetEvt, kEventCount };

  void Start() { state_machine_.Trigger(kStartEvt); }
  void End() { state_machine_.Trigger(kEndEvt); }
  void Reset() { state_machine_.Trigger(kResetEvt); }

 private:
  void DoStart() {}
  void DoTick() {}
  void DoEnd() {}
  void DoReset() {}
  void DoNop() {}

  using ClientStateMachine_t = aofsm::StateMachine<SimlpeClient>;

  ClientStateMachine_t state_machine_{
      this,
      {// Default-Action
       // {Src-State  Event   Dst-State Actions}
       {INITIAL_STATE, kStartEvt, MIDDLE_STATE, {&DoTick}},

       // Transitions
       // {Src-State  Event   Dst-State Actions}
       {INITIAL_STATE, kStartEvt, MIDDLE_STATE, {&DoStart, &DoTick}},
       {MIDDLE_STATE, kEndEvt, FINAL_STATE, {&DoTick, &DoEnd}},
       {FINAL_STATE, kResetEvt, INITIAL_STATE, &DoReset}}};
};

TEST(aofsm_StateMachineMultipleActions, instantiate) {
  SimlpeClient simple_client;
}
