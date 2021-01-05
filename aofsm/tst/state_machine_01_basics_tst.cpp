// copyright Yevgen
//
#include "aofsm/src/std_types.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "client1.h"

TEST(aofsm_StateMachineDescription_V2, GetTransitionData_INITIAL_STATE) {
  using Client1Context_t = Client1::StateMachine_t::Context_t;

  using Client1StateMachineDescription_t =
      Client1::StateMachine_t::StateMachineDescription_t;

  using TransitionData_t = Client1Context_t::TransitionData_t;

  Client1StateMachineDescription_t client_state_machine_description;

  const auto& trans_INITIAL_STATE_kStartAEvt =
      client_state_machine_description.GetTransitionData(Client1::INITIAL_STATE,
                                                         Client1::kStartAEvt);

  EXPECT_EQ(Client1::A_STATE, trans_INITIAL_STATE_kStartAEvt.dest_state);
  EXPECT_EQ(&Client1::DoStartA, trans_INITIAL_STATE_kStartAEvt.action);

  const auto& trans_INITIAL_STATE_kStartBEvt =
      client_state_machine_description.GetTransitionData(Client1::INITIAL_STATE,
                                                         Client1::kStartBEvt);

  EXPECT_EQ(Client1::B_STATE, trans_INITIAL_STATE_kStartBEvt.dest_state);
  EXPECT_EQ(&Client1::DoStartB, trans_INITIAL_STATE_kStartBEvt.action);

  const auto& trans_INITIAL_STATE_kEndEvt =
      client_state_machine_description.GetTransitionData(Client1::INITIAL_STATE,
                                                         Client1::kEndEvt);

  EXPECT_FALSE(Client1::StateMachine_t::IsValidState(
      trans_INITIAL_STATE_kEndEvt.dest_state));
  EXPECT_EQ(nullptr, trans_INITIAL_STATE_kEndEvt.action);
}
