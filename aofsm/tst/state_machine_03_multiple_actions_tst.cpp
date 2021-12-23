// copyright Yevgen
//
// Mehrere Aktionen pro Transition

#include "aofsm/src/state_machine.h"
#include "aofsm/src/std_types.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

// state machine
//
//  [INITIAL_STATE]+->(StartEvt -> [MIDDLE_STATE]
//                 |    \DoStart,
//                 |     DoTick)
//
//   states :
//            INITIAL_STATE, MIDDLE_STATE
//   events :
//            kStartEvt, kResetEvt
//   actions :
//             DoStart, DoTick
//   transitions:
//         INITIAL_STATE -> MIDDLE_STATE
//            kStartEvt/DoStart,DoTick
//
class SimlpeClient3 {
 public:
  enum State { INITIAL_STATE, MIDDLE_STATE, kStateCount };
  enum Event { kStartEvt, kEventCount };

  MOCK_METHOD(void, DoStart, (), ());
  MOCK_METHOD(void, DoTick, (), ());

  DECL_STATE_MACHINE(SimlpeClient3, state_machine_,
                     MAX_ACTIONS_PER_TRANSITION(2));
};

DEF_STATE_MACHINE(
    SimlpeClient3, state_machine_,
    // Transitions
    // {Src-State , Event , Dst-State , Actions}
    {INITIAL_STATE, kStartEvt, MIDDLE_STATE, {&DoStart, &DoTick}});

TEST(aofsm_StateMachineMultipleActions,
     MastCallAllActionsOnTransitionWithMultipleActions) {
  // Arrange
  SimlpeClient3 simple_client;
  EXPECT_CALL(simple_client, DoStart).Times(1);
  EXPECT_CALL(simple_client, DoTick).Times(1);

  // Act
  simple_client.state_machine_.Trigger(SimlpeClient3::Event::kStartEvt);

  // Assert
  EXPECT_EQ(simple_client.state_machine_.GetCurrentState(),
            SimlpeClient3::State::MIDDLE_STATE);
}
