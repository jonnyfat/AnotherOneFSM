// copyright Yevgen
//
// Mehrere Aktionen pro Transition

#include "aofsm/src/std_types.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "aofsm/src/state_machine.h"

// state machine
//
//  [INITIAL_STATE]+->(StartEvt -> [MIDDLE_STATE] -> (EndEvt  +->
//  [FINAL_STATE]
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

  MOCK_METHOD(void, DoStart, (), ());
  MOCK_METHOD(void, DoTick, (), ());
  MOCK_METHOD(void, DoEnd, (), ());
  MOCK_METHOD(void, DoReset, (), ());
  MOCK_METHOD(void, DoNop, (), ());

  DECL_STATE_MACHINE_WITH_MULT_ACTIONS(SimlpeClient3, state_machine_, 2);
};

DEF_STATE_MACHINE(SimlpeClient3, state_machine_){
    {// Transitions
     // {Src-State  Event   Dst-State Actions}
     {INITIAL_STATE, kStartEvt, MIDDLE_STATE, {&DoStart, &DoTick}},
     {MIDDLE_STATE, kEndEvt, FINAL_STATE, {&DoTick, &DoEnd}},
     {FINAL_STATE, kResetEvt, INITIAL_STATE, &DoReset}}};

TEST(aofsm_StateMachineMultipleActions,
     MastCallAllActionsOnTransitionWithMultipleActions) {
  // Arrange
  SimlpeClient3 simple_client;
  EXPECT_CALL(simple_client, DoStart).Times(1);
  EXPECT_CALL(simple_client, DoTick).Times(1);

  EXPECT_CALL(simple_client, DoEnd).Times(0);
  EXPECT_CALL(simple_client, DoReset).Times(0);
  EXPECT_CALL(simple_client, DoNop).Times(0);

  // Act
  simple_client.state_machine_.Trigger(SimlpeClient3::Event::kStartEvt);

  // Assert
  EXPECT_EQ(simple_client.state_machine_.GetCurrentState(),
            SimlpeClient3::State::MIDDLE_STATE);
}
