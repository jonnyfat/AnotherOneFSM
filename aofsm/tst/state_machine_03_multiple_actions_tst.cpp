// copyright Yevgen
//
// Mehrere Aktionen pro Transition

#include "aofsm/src/std_types.h"



#include "aofsm/src/state_machine_aliases.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

// state machine
//
//  [INITIAL_STATE]+->(StartEvt -> [MIDDLE_STATE] -> (EndEvt  +-> [FINAL_STATE]
//                 |    \DoStart,                     \DoTick, |
//                 |     DoTick)                        DoEnd)|
//                                  <-ResetEvt
//                                    \DoReset
//   states :
//            INITIAL_STATE, MIDDLE_STATE, FINAL_STATE
//   events :
//            kStartEvt, kEndEvt, kResetEvt
//   actions :
//             DoStart, DoTick, DoEnd, DoReset,
//   transitions:
//         INITIAL_STATE -> MIDDLE_STATE
//            kStartEvt/DoStart,DoTick
//
//         MIDDLE_STATE -> FINAL_STATE
//              kEndEvt/DoTick,DoEnd
//
//         FINAL_STATE -> B_STATE
//              kResetEvt/DoReset
//
class SimlpeClient3 {
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

  using StateMachine_t = aofsm::StateMachineWithCustomActions<SimlpeClient3, 2>;

  using StateMachineDescription_t = StateMachine_t::StateMachineDescription_t;

  static const StateMachineDescription_t state_machine_description_;

  StateMachine_t state_machine_{this, state_machine_description_};
};

const SimlpeClient3::StateMachineDescription_t
    SimlpeClient3::state_machine_description_{
        {// Transitions
         // {Src-State  Event   Dst-State Actions}
         {INITIAL_STATE, kStartEvt, MIDDLE_STATE, {&DoStart, &DoTick}},
         {MIDDLE_STATE, kEndEvt, FINAL_STATE, {&DoTick, &DoEnd}},
         {FINAL_STATE, kResetEvt, INITIAL_STATE, &DoReset}}};

TEST(aofsm_StateMachineMultipleActions, instantiate) {
  SimlpeClient3 simple_client;
}
