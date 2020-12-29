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

#include "aofsm/src/state_machine_context.h"

#include "aofsm/src/states_list.h"

using aofsm::internal::ValueHolder;

using aofsm::internal::InvalidAction;
using aofsm::internal::InvalidState;

using aofsm::TransitionData;

using aofsm::TransitionDescription;

using aofsm::StateMachineContext;

using aofsm::internal::StatesList;

// Enthält Informationen über Transitionen der State-Machine.
// Für ein Event in einem Zustand stellt Übergansigsinformation:
//   - neuer Zustand
//   - Aktion, welche bei Übergang durchgeführt wird
template <typename Context>
class StateMachineDescription {
 public:
  using State_t = typename Context::State_t;
  using Event_t = typename Context::Event_t;
  using Action_t = typename Context::Action_t;

  // TransitionDescription: Definition von Client-State-Machine
  template <State_t src_state, Event_t event>
  using TransitionDescription_t =
      TransitionDescription<Context, src_state, event>;

  using TransitionData_t = TransitionData<Context>;

  template <State_t state, Event_t event>
  static constexpr TransitionData_t GetTransitionData() {
    return TransitionDescription_t<state, event>::transition_data;
  }

  template <size_t state_idx, size_t event_idx>
  static constexpr TransitionData_t SE() {
    return TransitionDescription_t<static_cast<State_t>(state_idx),
                                   static_cast<Event_t>(
                                       event_idx)>::transition_data;
  };

  using StateTransitionDataArray_t = TransitionData_t[Event_t::kEventCount];

  using TransitionDataMatrix_t =
      StateTransitionDataArray_t[State_t::kStateCount];

  static const TransitionData_t& GetTransitionData(State_t state,
                                                   Event_t event) {
    return transition_data_matrix[state][event];
  }

  static const TransitionDataMatrix_t transition_data_matrix;
};

template <typename Context>
const typename StateMachineDescription<Context>::TransitionDataMatrix_t
    StateMachineDescription<Context>::transition_data_matrix;

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
  using Guard_t = bool (Client::*)(ActionParameterTypes...);

  using Context_t =
      StateMachineContext<Client, State_t, Event_t, Action_t, Guard_t,
                          State_t::kStateCount, Event_t::kEventCount>;

  using TransitionData_t = TransitionData<Context_t>;

  using StateMachineDescription_t = StateMachineDescription<Context_t>;

  static constexpr bool IsValidState(State_t state) {
    return state >= 0 && state < State_t::kStateCount;
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
  struct aofsm::TransitionDescription<FSM_DESCR::Context_t,                   \
                                      FSM_DESCR::State_t::SRC_STATE,          \
                                      FSM_DESCR::Event_t::EVENT> {            \
    static constexpr TransitionData<FSM_DESCR::Context_t> transition_data = { \
        FSM_DESCR::State_t::DST_STATE, &FSM_DESCR::Client_t::ACTION};         \
  };

using Client1Context_t = Client1::StateMachine_t::Context_t;

using ClientState_t = Client1Context_t::State_t;

using ClientEvent_t = Client1Context_t::Event_t;

using ClientAction_t = Client1Context_t::Action_t;

using TransitionData_t = Client1::StateMachine_t::TransitionData_t;

//----------------------------------------------------------------------------------
//   DEF_TRANS
DEF_TRANS(Client1::StateMachine_t, INITIAL_STATE, kStartAEvt, A_STATE, DoStartA)

DEF_TRANS(Client1::StateMachine_t, INITIAL_STATE, kStartBEvt, B_STATE, DoStartB)

DEF_TRANS(Client1::StateMachine_t, B_STATE, kEndEvt, FINAL_STATE, DoEndB)

template <>
const Client1::StateMachine_t::StateMachineDescription_t::TransitionDataMatrix_t
    Client1::StateMachine_t::StateMachineDescription_t::transition_data_matrix =
        {{
             {SE<0, 0>()},
             {SE<0, 1>()},
             {SE<0, 2>()},
         },
         {
             {SE<1, 0>()},
             {SE<1, 1>()},
             {SE<1, 2>()},
         },
         {
             {SE<2, 0>()},
             {SE<2, 1>()},
             {SE<2, 2>()},
         },
         {
             {SE<3, 0>()},
             {SE<3, 1>()},
             {SE<3, 2>()},
         }};

TEST(aofsm_StateMachine, StateMachineDescription_CompileTime) {
  Client1::StateMachine_t::StateMachineDescription_t state_machine_description;

  auto transition_data1 =
      state_machine_description
          .GetTransitionData<Client1::INITIAL_STATE, Client1::kStartAEvt>();

  EXPECT_TRUE(transition_data1.IsValidState());
  EXPECT_EQ(transition_data1.dest_state, Client1::A_STATE);
  EXPECT_EQ(transition_data1.action, &Client1::DoStartA);

  auto transition_data2 =
      state_machine_description
          .GetTransitionData<Client1::INITIAL_STATE, Client1::kStartBEvt>();

  EXPECT_TRUE(transition_data2.IsValidState());
  EXPECT_EQ(transition_data2.dest_state, Client1::B_STATE);
  EXPECT_EQ(transition_data2.action, &Client1::DoStartB);

  auto transition_data3 =
      state_machine_description
          .GetTransitionData<Client1::INITIAL_STATE, Client1::kEndEvt>();

  EXPECT_FALSE(transition_data3.IsValidState());
  EXPECT_EQ(transition_data3.action, nullptr);
}

TEST(aofsm_StateMachine, StateMachineDescription_RunTime) {
  Client1::StateMachine_t::StateMachineDescription_t state_machine_description;

  auto transition_data1 = state_machine_description.GetTransitionData(
      Client1::INITIAL_STATE, Client1::kStartAEvt);

  EXPECT_TRUE(transition_data1.IsValidState());
  EXPECT_EQ(transition_data1.dest_state, Client1::A_STATE);
  EXPECT_EQ(transition_data1.action, &Client1::DoStartA);

  auto transition_data2 = state_machine_description.GetTransitionData(
      Client1::INITIAL_STATE, Client1::kStartBEvt);

  EXPECT_TRUE(transition_data2.IsValidState());
  EXPECT_EQ(transition_data2.dest_state, Client1::B_STATE);
  EXPECT_EQ(transition_data2.action, &Client1::DoStartB);

  auto transition_data3 = state_machine_description.GetTransitionData(
      Client1::INITIAL_STATE, Client1::kEndEvt);

  EXPECT_FALSE(transition_data3.IsValidState());
  EXPECT_EQ(transition_data3.action, nullptr);

  auto transition_data4 = state_machine_description.GetTransitionData(
      Client1::B_STATE, Client1::kEndEvt);

  EXPECT_TRUE(transition_data4.IsValidState());
  EXPECT_EQ(transition_data4.dest_state, Client1::FINAL_STATE);
  EXPECT_EQ(transition_data4.action, &Client1::DoEndB);
}

//---------------------------------------
// TransitionDataArrayHolderConstexpr
template <typename TransitionData, typename... TransitionDescriptions>
struct TransitionDataArrayHolderConstexpr {
  static constexpr TransitionData data[sizeof...(TransitionDescriptions)] = {
      TransitionDescriptions::transition_data...};
};

template <typename TransitionData, typename... TransitionDescriptions>
constexpr TransitionData TransitionDataArrayHolderConstexpr<
    TransitionData,
    TransitionDescriptions...>::data[sizeof...(TransitionDescriptions)];

template <ClientState_t src_state, ClientEvent_t event>
using TransitionDescription_t =
    Client1::StateMachine_t::StateMachineDescription_t::TransitionDescription_t<
        src_state, event>;

TransitionDataArrayHolderConstexpr<TransitionData_t>
    empty_transition_data_array;

TransitionDataArrayHolderConstexpr<
    TransitionData_t,
    TransitionDescription_t<Client1::INITIAL_STATE, Client1::kStartAEvt> >
    one_el_transition_data_array;

TEST(aofsm_StateMachine, ArrayHolder2) {
  EXPECT_EQ(Client1::A_STATE, one_el_transition_data_array.data[0].dest_state);
  EXPECT_EQ(&Client1::DoStartA, one_el_transition_data_array.data[0].action);
}

//-----------------------
// TransitionDataArrayHolder
template <typename Context, typename... TransitionDescriptions>
struct TransitionDataArrayHolder {
  static const TransitionData<Context> data[sizeof...(TransitionDescriptions)];
};

template <typename Context, typename... TransitionDescriptions>
const TransitionData<Context> TransitionDataArrayHolder<
    Context,
    TransitionDescriptions...>::data[sizeof...(TransitionDescriptions)] = {
    TransitionDescriptions::transition_data...};

//-----------------------
// TransitionDataArray
template <typename Context, typename... TransitionDescriptions>
struct TransitionDataArray {
  static constexpr TransitionData<Context> transition_data[sizeof...(
      TransitionDescriptions)] = {{TransitionDescriptions::transition_data}...};
};

template <typename Context, typename... TransitionDescriptions>
constexpr TransitionData<Context>
    TransitionDataArray<Context, TransitionDescriptions...>::transition_data
        [sizeof...(TransitionDescriptions)];

//-----------------------
// GenerateTransitionDataArrayImpl
template <size_t EVENT_INDEX, typename Context,
          template <size_t> class GetTransition,
          typename... TransitionDescriptions>
struct GenerateTransitionDataArrayImpl {
  using TransitionDataArray_t = typename GenerateTransitionDataArrayImpl<
      EVENT_INDEX - 1, Context, GetTransition,
      typename GetTransition<EVENT_INDEX>::TransitionDescription_t,
      TransitionDescriptions...>::TransitionDataArray_t;
};

template <typename Context, template <size_t> class GetTransition,
          typename... TransitionDescriptions>
struct GenerateTransitionDataArrayImpl<0, Context, GetTransition,
                                       TransitionDescriptions...> {
  using TransitionDataArray_t =
      TransitionDataArray<Context,
                          typename GetTransition<0>::TransitionDescription_t,
                          TransitionDescriptions...>;
};

template <size_t EVENT_COUNT, typename Context,
          template <size_t> class GetTransition>
struct GenerateTransitionDataArray {
  using TransitionDataArray_t = typename GenerateTransitionDataArrayImpl<
      EVENT_COUNT - 1, Context, GetTransition>::TransitionDataArray_t;
};

// Enthält Array mit TransitionData<> für den Zustand state.
template <typename Context, typename Context::State_t state>
struct StateTransitionsArray {
  using Event_t = typename Context::Event_t;
  // Meta-Function:
  //  Parameter: event_index
  //  Rückgabewert: Typ : Template-Instanz von TransitionDescription für den
  //  Event
  template <size_t EVENT_INDEX>
  struct GetTransition {
    using TransitionDescription_t =
        TransitionDescription<Context, state,
                              static_cast<Event_t>(EVENT_INDEX)>;
  };

  typename GenerateTransitionDataArray<
      static_cast<size_t>(Context::kEventCount), Context,
      GetTransition>::TransitionDataArray_t transition_data_array;

  const TransitionData<Context>& GetTransitionData(Event_t event) {
    return transition_data_array.transition_data[event];
  }
};

TEST(aofsm_StateMachine, StateTransitionsArray) {
  StateTransitionsArray<Client1Context_t, Client1::INITIAL_STATE>
      TransitionsArray_INITIAL_STATE;

  const TransitionData<Client1Context_t>& trans_INITIAL_STATE_kStartAEvt =
      TransitionsArray_INITIAL_STATE.GetTransitionData(Client1::kStartAEvt);

  EXPECT_EQ(Client1::A_STATE, trans_INITIAL_STATE_kStartAEvt.dest_state);
  EXPECT_EQ(&Client1::DoStartA, trans_INITIAL_STATE_kStartAEvt.action);

  const TransitionData<Client1Context_t>& trans_INITIAL_STATE_kStartBEvt =
      TransitionsArray_INITIAL_STATE.GetTransitionData(Client1::kStartBEvt);

  EXPECT_EQ(Client1::B_STATE, trans_INITIAL_STATE_kStartBEvt.dest_state);
  EXPECT_EQ(&Client1::DoStartB, trans_INITIAL_STATE_kStartBEvt.action);

  const TransitionData<Client1Context_t>& trans_INITIAL_STATE_kEndEvt =
      TransitionsArray_INITIAL_STATE.GetTransitionData(Client1::kEndEvt);

  EXPECT_EQ(InvalidState<ClientState_t>::value,
            trans_INITIAL_STATE_kEndEvt.dest_state);
  EXPECT_EQ(nullptr, trans_INITIAL_STATE_kEndEvt.action);
}
