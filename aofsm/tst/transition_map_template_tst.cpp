// copyright Yevgen
//

#include <cstddef>

using std::size_t;

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "aofsm/src/value_holder.h"

#include "aofsm/src/invalid_state.h"
#include "aofsm/src/invalid_action.h"

#include "aofsm/src/transition_data.h"

#include "aofsm/src/transition_description.h"

#include "aofsm/src/states_list.h"

using aofsm::internal::ValueHolder;

using aofsm::internal::InvalidAction;
using aofsm::internal::InvalidState;

using aofsm::TransitionData;

using aofsm::TransitionDescription;

using aofsm::internal::StatesList;

template <typename State, typename Event, typename Action>
class StateMachineDescription {
 public:
  using State_t = State;
  using Event_t = Event;
  using Action_t = Action;

  //----------------------------------------
  //--
  using StateMachineStates_t = StatesList<State_t, Event_t, Action_t>;

  template <State state, Event event>
  static constexpr TransitionData<State, Action> GetTransitionDataOld() {
    return StateMachineStates_t::template GetTransitionData<state, event>();
  }
  //--
  //----------------------------------------

  //----------------------------------------
  //--
  template <State src_state, Event event>
  using TransitionMapEntry_t =
      TransitionDescription<State, Event, Action, src_state, event>;

  template <State state, Event event>
  static constexpr TransitionData<State, Action> GetTransitionData() {
    return TransitionMapEntry_t<state, event>::transition_data;
  }
  //--
  //----------------------------------------

  using TransitionData_t = TransitionData<State_t, Action_t>;

  using StateTransitionDataArray_t = TransitionData_t[Event::kEventCount];

  using TransitionDataMatrix_t = StateTransitionDataArray_t[State::kStateCount];

  static const TransitionData_t& GetTransitionData(State_t state,
                                                   Event_t event) {
    return (transition_data_matrix[state])[event];
  }

  static const TransitionDataMatrix_t transition_data_matrix;
};

template <typename Client, typename State = typename Client::State,
          typename Event = typename Client::Event,
          typename... ActionParameterTypes>
class StateMachine {
 public:
  using Client_t = Client;
  using State_t = State;
  using Event_t = Event;
  // Pointer auf Member-Methode des Clients, welche bei einer Transition als
  // Action aufgerufen wird.
  using Action_t = void (Client::*)(ActionParameterTypes...);

  using StateMachineDescription_t =
      StateMachineDescription<State_t, Event_t, Action_t>;

  static constexpr bool IsValidState(State_t state) {
    return StateMachineDescription_t::IsValidState(state);
  }

  State_t current_state_{State_t::INITIAL_STATE};
};

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

  using Action_t = void (Client1::*)();

  void DoStartA() {}
  void DoStartB() {}
  void DoEndA() {}
  void DoEndB() {}

  //                             {{INITIAL_STATE, kStartAEvt, A_STATE,
  //                             &DoStartA},
  //                              {INITIAL_STATE, kStartBEvt, B_STATE,
  //                              &DoStartB}, {A_STATE, kEndEvt, FINAL_STATE,
  //                              &DoEndA}, {B_STATE, kEndEvt, FINAL_STATE,
  //                              &DoEndB}}};

  using StateMachine_t = StateMachine<Client1>;
  using StateMachineDescription_t = StateMachine_t::StateMachineDescription_t;
};

#define DEF_TRANS(FSM_DESCR, SRC_STATE, EVENT, DST_STATE, ACTION)             \
  template <>                                                                 \
  struct aofsm::TransitionDescription<                                        \
      FSM_DESCR::State_t, FSM_DESCR::Event_t, FSM_DESCR::Action_t,            \
      FSM_DESCR::State_t::SRC_STATE, FSM_DESCR::Event_t::EVENT> {             \
    static constexpr TransitionData<FSM_DESCR::State_t, FSM_DESCR ::Action_t> \
        transition_data = {FSM_DESCR::State_t::DST_STATE,                     \
                           &FSM_DESCR::Client_t::ACTION};                     \
  };

DEF_TRANS(Client1::StateMachine_t, INITIAL_STATE, kStartAEvt, A_STATE, DoStartA)

DEF_TRANS(Client1::StateMachine_t, INITIAL_STATE, kStartBEvt, B_STATE, DoStartB)

TEST(aofsm_StateMachine, StateEventsInstantiation) {
  Client1::StateMachine_t::StateMachineDescription_t state_machine_states;

  auto transition_data1 =
      state_machine_states
          .GetTransitionData<Client1::INITIAL_STATE, Client1::kStartAEvt>();

  EXPECT_TRUE(transition_data1.IsValidState());
  EXPECT_EQ(transition_data1.dest_state, Client1::A_STATE);
  EXPECT_EQ(transition_data1.action, &Client1::DoStartA);

  auto transition_data2 =
      state_machine_states
          .GetTransitionData<Client1::INITIAL_STATE, Client1::kStartBEvt>();

  EXPECT_TRUE(transition_data2.IsValidState());
  EXPECT_EQ(transition_data2.dest_state, Client1::B_STATE);
  EXPECT_EQ(transition_data2.action, &Client1::DoStartB);

  auto transition_data3 =
      state_machine_states
          .GetTransitionData<Client1::INITIAL_STATE, Client1::kEndEvt>();

  EXPECT_FALSE(transition_data3.IsValidState());
  EXPECT_EQ(transition_data3.action, nullptr);
}

TEST(aofsm_StateMachine, TransitionDescription) {
  Client1::StateMachine_t::StateMachineDescription_t state_machine_states;

  auto transition_data1 =
      state_machine_states
          .GetTransitionData<Client1::INITIAL_STATE, Client1::kStartAEvt>();

  EXPECT_TRUE(transition_data1.IsValidState());
  EXPECT_EQ(transition_data1.dest_state, Client1::A_STATE);
  EXPECT_EQ(transition_data1.action, &Client1::DoStartA);

  auto transition_data2 =
      state_machine_states
          .GetTransitionData<Client1::INITIAL_STATE, Client1::kStartBEvt>();

  EXPECT_TRUE(transition_data2.IsValidState());
  EXPECT_EQ(transition_data2.dest_state, Client1::B_STATE);
  EXPECT_EQ(transition_data2.action, &Client1::DoStartB);

  auto transition_data3 =
      state_machine_states
          .GetTransitionData<Client1::INITIAL_STATE, Client1::kEndEvt>();

  EXPECT_FALSE(transition_data3.IsValidState());
  EXPECT_EQ(transition_data3.action, nullptr);
}

TEST(aofsm_StateMachine, Dummy) {}
