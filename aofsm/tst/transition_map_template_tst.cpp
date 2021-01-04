// copyright Yevgen
//
#include "aofsm/src/std_types.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "aofsm/tst/client_v2.h"

TEST(aofsm_StateMachineDescription_V2, GetTransitionData_INITIAL_STATE) {
  using Client1Context_t = ClientV2::StateMachine_t::Context_t;

  using Client1StateMachineDescription_t =
      ClientV2::StateMachine_t::StateMachineDescription_t;

  using TransitionData_t = ClientV2::StateMachine_t::TransitionData_t;

  Client1StateMachineDescription_t client_state_machine_description;

  const auto& trans_INITIAL_STATE_kStartAEvt =
      client_state_machine_description.GetTransitionData(
          ClientV2::INITIAL_STATE, ClientV2::kStartAEvt);

  EXPECT_EQ(ClientV2::A_STATE, trans_INITIAL_STATE_kStartAEvt.dest_state);
  EXPECT_EQ(&ClientV2::DoStartA, trans_INITIAL_STATE_kStartAEvt.action);

  const auto& trans_INITIAL_STATE_kStartBEvt =
      client_state_machine_description.GetTransitionData(
          ClientV2::INITIAL_STATE, ClientV2::kStartBEvt);

  EXPECT_EQ(ClientV2::B_STATE, trans_INITIAL_STATE_kStartBEvt.dest_state);
  EXPECT_EQ(&ClientV2::DoStartB, trans_INITIAL_STATE_kStartBEvt.action);

  const auto& trans_INITIAL_STATE_kEndEvt =
      client_state_machine_description.GetTransitionData(
          ClientV2::INITIAL_STATE, ClientV2::kEndEvt);

  EXPECT_FALSE(ClientV2::StateMachine_t::IsValidState(
      trans_INITIAL_STATE_kEndEvt.dest_state));
  EXPECT_EQ(nullptr, trans_INITIAL_STATE_kEndEvt.action);
}
