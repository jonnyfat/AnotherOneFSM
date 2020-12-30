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

//-----------------------
// TransitionDataHolder
// - Instantiiert ein TransitionData<>-Array
// - Intialisiert jedes Element mit entsprechenden TransitionDescription
template <typename Context, typename... TransitionDescription>
struct TransitionDataHolder {
  static constexpr TransitionData<Context> transition_data[sizeof...(
      TransitionDescription)] = {{TransitionDescription::transition_data}...};
};

template <typename Context, typename... TransitionDescription>
constexpr TransitionData<Context>
    TransitionDataHolder<Context, TransitionDescription...>::transition_data
        [sizeof...(TransitionDescription)];

//-----------------------
// GenerateTransitionDataImpl
template <size_t EVENT_INDEX, typename Context,
          template <size_t> class GetTransition,
          typename... TransitionDescription>
struct GenerateTransitionDataImpl {
  using TransitionDataHolder_t = typename GenerateTransitionDataImpl<
      EVENT_INDEX - 1, Context, GetTransition,
      typename GetTransition<EVENT_INDEX>::TransitionDescription_t,
      TransitionDescription...>::TransitionDataHolder_t;
};

template <typename Context, template <size_t> class GetTransition,
          typename... TransitionDescription>
struct GenerateTransitionDataImpl<0, Context, GetTransition,
                                  TransitionDescription...> {
  using TransitionDataHolder_t =
      TransitionDataHolder<Context,
                           typename GetTransition<0>::TransitionDescription_t,
                           TransitionDescription...>;
};

template <size_t EVENT_COUNT, typename Context,
          template <size_t> class GetTransition>
struct GenerateTransitionData {
  using TransitionDataHolder_t = typename GenerateTransitionDataImpl<
      EVENT_COUNT - 1, Context, GetTransition>::TransitionDataHolder_t;
};

// Enthält Array mit TransitionData<> pro jeden Event der State-Machine für ein
// Zustand der State-Machine.
template <typename Context, typename Context::State_t state>
struct StateTransitions {
  using Event_t = typename Context::Event_t;

  using StateTransitionData_t = TransitionData<Context>[Context::kEventCount];

  using StateTransitionDataPtr_t = StateTransitionData_t const*;

  // Meta-Function (Rückgabewert ist ein Typ):
  //  Parameter: event_index
  //  Rückgabewert:
  //    TransitionDescription-Template-Instanz für den Event = EVENT_INDEX
  template <size_t EVENT_INDEX>
  struct GetTransitionDescription {
    using TransitionDescription_t =
        TransitionDescription<Context, state,
                              static_cast<Event_t>(EVENT_INDEX)>;
  };

  // Mit Hilfe von Meta-Function GenerateTransitionDataArray wird
  // TransitionDataHolder-Template-Instanz erzeugt.
  using TransitionDataHolder_t = typename GenerateTransitionData<
      static_cast<size_t>(Context::kEventCount), Context,
      GetTransitionDescription>::TransitionDataHolder_t;

  const TransitionData<Context>& GetTransitionData(Event_t event) {
    return TransitionDataHolder_t::transition_data[event];
  }
};

TEST(aofsm_StateMachine_V2, StateTransitions) {
  StateTransitions<Client1Context_t, Client1::INITIAL_STATE>
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

////-----------------------
//// TransitionDataHolder
// template <typename Context, typename... TransitionDescriptions>
// struct StateTransitionsArray {
//  static constexpr StateTransitions<Context> transition_data[sizeof...(
//      TransitionDescriptions)] =
//      {{TransitionDescriptions::transition_data}...};
//};
//
// template <typename Context, typename... TransitionDescriptions>
// constexpr TransitionData<Context>
//    TransitionDataHolder<Context, TransitionDescriptions...>::transition_data
//        [sizeof...(TransitionDescriptions)];
//
////-----------------------
//// GenerateTransitionDataArrayImpl
// template <size_t EVENT_INDEX, typename Context,
//          template <size_t> class GetTransition,
//          typename... TransitionDescriptions>
// struct GenerateTransitionDataArrayImpl {
//  using TransitionDataArray_t = typename GenerateTransitionDataArrayImpl<
//      EVENT_INDEX - 1, Context, GetTransition,
//      typename GetTransition<EVENT_INDEX>::TransitionDescription_t,
//      TransitionDescriptions...>::TransitionDataArray_t;
//};
//
// template <typename Context, template <size_t> class GetTransition,
//          typename... TransitionDescriptions>
// struct GenerateTransitionDataArrayImpl<0, Context, GetTransition,
//                                       TransitionDescriptions...> {
//  using TransitionDataArray_t =
//      TransitionDataHolder<Context,
//                          typename GetTransition<0>::TransitionDescription_t,
//                          TransitionDescriptions...>;
//};
//
// template <size_t EVENT_COUNT, typename Context,
//          template <size_t> class GetTransition>
// struct GenerateTransitionDataArray {
//  using TransitionDataArray_t = typename GenerateTransitionDataArrayImpl<
//      EVENT_COUNT - 1, Context, GetTransition>::TransitionDataArray_t;
//};
//
// Enthält Array mit StateTransitions<> pro jeden Zustand der
// State-Machine.
// template <typename Context>
// struct StateMachineTransitions {
//  using State_t = typename Context::State_t;
//  using Event_t = typename Context::Event_t;
//
//  using StateTransitionData_t = TransitionData<Context>[Context::kEventCount];
//
//  using StateTransitionDataPtr_t = StateTransitionData_t const*;
//
//  // Meta-Function:
//  //  Parameter: state_index
//  //  Rückgabewert: Typ : Template-Instanz von StateTransitionsArray für
//  //  den State
//  template <size_t STATE_INDEX>
//  struct GetStateTransitions {
//    using StateTransitions_t =
//        StateTransitions<Context, static_cast<State_t>(STATE_INDEX)>;
//  };
//
//  // Mit Hilfe von Meta-Function GetStateMachineTransitions wird
//  // StateTransitionsArray-Template-Instanz erzeugt.
//  using StateTransitionsArray_t = typename GenerateStateTransitionsArray<
//      static_cast<size_t>(Context::kStateCount), Context,
//      GetStateTransitions>::StateTransitionsArray_t;
//
//  static constexpr StateTransitionsArray_t state_transitions_array{};
//
//  static constexpr StateTransitionDataPtr_t state_transition_data_ptr =
//      &(StateTransitionsArray_t);
//
//  const TransitionData<Context>& GetTransitionData(State_t state,
//                                                   Event_t event) {
//    return state_transitions_array.state_transitions[state].GetTransitionData(
//        event);
//  }
//};

TEST(aofsm_StateMachine_V2, StateMachineTransitions) {}

struct Data {
  size_t data;
};

using DataArrayOf2 = Data[2];

TEST(aofsm_StateMachine_V2, DummyXXX01) {
  Data dataof2[2] = {{88}, {99}};

  DataArrayOf2* tmp = &dataof2;

  Data data0 = (*tmp)[0];
  Data data1 = (*tmp)[1];

  EXPECT_EQ(data0.data, 88);
  EXPECT_EQ(data1.data, 99);
}

struct DataHolder {
  static constexpr Data data1[2] = {101, 202};
  static constexpr Data data2[2] = {303, 404};
};

struct DataArrayOf2Holder {
  static constexpr DataArrayOf2 const* data1[2] = {&DataHolder::data1,
                                                   &DataHolder::data2};
};

TEST(aofsm_StateMachine_V2, DummyXXX02) {
  Data data0 = (*DataArrayOf2Holder::data1[0])[0];
  Data data1 = (*DataArrayOf2Holder::data1[0])[1];
  Data data2 = (*DataArrayOf2Holder::data1[1])[0];
  Data data3 = (*DataArrayOf2Holder::data1[1])[1];

  EXPECT_EQ(data0.data, 101);
  EXPECT_EQ(data1.data, 202);
  EXPECT_EQ(data2.data, 303);
  EXPECT_EQ(data3.data, 404);
}
