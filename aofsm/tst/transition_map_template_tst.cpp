// copyright Yevgen
//
#include <cstddef>

using std::size_t;

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "aofsm/src/invalid_state.h"

#include "aofsm/src/transition_data.h"

#include "aofsm/src/transition_description.h"

#include "aofsm/src/state_machine_context.h"

using aofsm::internal::InvalidState;

using aofsm::TransitionData;

using aofsm::TransitionDescription;

using aofsm::StateMachineContext;

//-----------------------
// TransitionDataHolder
// - instantiiert ein TransitionData<>-Array
// - intialisiert jedes TransitionData<>-Element mit
// TransitionDescription, welche als Template-Parameter angegeben werden muss
template <typename Context, typename... TransitionDescriptions>
struct TransitionDataHolder {
  // Array mit Transition-Informationen
  static constexpr TransitionData<Context> transition_data[sizeof...(
      TransitionDescriptions)] = {{TransitionDescriptions::transition_data}...};
  // Anzahl der Elemente in Array transition_data
  enum : size_t { kTransitionDataCount = sizeof...(TransitionDescriptions) };
};

template <typename Context, typename... TransitionDescriptions>
constexpr TransitionData<Context>
    TransitionDataHolder<Context, TransitionDescriptions...>::transition_data
        [sizeof...(TransitionDescriptions)];

//-----------------------
// GenerateTransitionDataImpl: definition
template <size_t kFirstEventIndex, size_t kLastEventIndex, typename Context,
          typename Context::State_t state, typename... TransitionDescriptions>
struct GenerateTransitionDataImpl {
  using TransitionDescription_t = TransitionDescription<
      Context, state, static_cast<typename Context::Event_t>(kLastEventIndex)>;

  using TransitionDataHolder_t = typename GenerateTransitionDataImpl<
      kFirstEventIndex, kLastEventIndex - 1, Context, state,
      TransitionDescription_t,
      TransitionDescriptions...>::TransitionDataHolder_t;
};

// GenerateTransitionDataImpl: terminal spezialisation
template <size_t kFirstEventIndex, typename Context,
          typename Context::State_t state, typename... TransitionDescriptions>
struct GenerateTransitionDataImpl<kFirstEventIndex, kFirstEventIndex, Context,
                                  state, TransitionDescriptions...> {
  using TransitionDescription_t = TransitionDescription<
      Context, state, static_cast<typename Context::Event_t>(kFirstEventIndex)>;

  using TransitionDataHolder_t =
      TransitionDataHolder<Context, TransitionDescription_t,
                           TransitionDescriptions...>;
};

// GenerateTransitionData ist Meta-Function
//   Parameter:
//      Context - Kontext der State-Machine
//      state  - Zustand-Id eines Zustandes der State-Machine
//   Rückgabewert ist ein Typ:
//     TransitionDataHolder_t-Template-Instanz für den Zustand state
template <typename Context, typename Context::State_t state>
struct GenerateTransitionData {
  enum : size_t {
    kFirstEventIndex = 0,
    kLastEventIndex = Context::kEventCount - 1
  };

  using TransitionDataHolder_t =
      typename GenerateTransitionDataImpl<kFirstEventIndex, kLastEventIndex,
                                          Context,
                                          state>::TransitionDataHolder_t;
};

//--------------------------------------------
// Enthält Array mit TransitionData<> pro jeden Event der State-Machine für ein
// Zustand der State-Machine.
template <typename Context, typename Context::State_t state>
struct StateTransitions {
  // Mit Hilfe von Meta-Function GenerateTransitionDataArray wird
  // TransitionDataHolder-Template-Instanz erzeugt.
  using TransitionDataHolder_t =
      typename GenerateTransitionData<Context, state>::TransitionDataHolder_t;

  static_assert(
      static_cast<size_t>(TransitionDataHolder_t::kTransitionDataCount) ==
      static_cast<size_t>(Context::kEventCount));
};

//-----------------------
// StatesPtrArrayHolder
// - Instantiiert ein Context::StateTransitionArrayPtr_t-Array
// - Intialisiert jedes Context::StateTransitionArrayPtr_t-Element mit
// entsprechenden Initalwert aus Templateparameter-Pack init_values
template <typename Context,
          typename Context::StateTransitionArrayPtr_t... init_values>
struct StatesPtrArrayHolder {
  static constexpr typename Context::StateTransitionArrayPtr_t
      states[sizeof...(init_values)] = {init_values...};

  enum : size_t { kStatesCount = sizeof...(init_values) };

  static constexpr const typename Context::StateTransitionArray_t&
  GetStateTransitionArray(typename Context::State_t state) {
    return *states[state];
  }
};

template <typename Context,
          typename Context::StateTransitionArrayPtr_t... init_values>
constexpr typename Context::StateTransitionArrayPtr_t StatesPtrArrayHolder<
    Context, init_values...>::states[sizeof...(init_values)];

//-----------------------
// GenerateStatesPtrArrayImpl
template <size_t kFirstStateIndex, size_t kLastStateIndex, typename Context,
          typename Context::StateTransitionArrayPtr_t... init_values>
struct GenerateStatesPtrArrayImpl {
  using TransitionDataHolder_t =
      typename StateTransitions<Context,
                                static_cast<typename Context::State_t>(
                                    kLastStateIndex)>::TransitionDataHolder_t;

  using StatesPtrArrayHolder_t = typename GenerateStatesPtrArrayImpl<
      kFirstStateIndex, kLastStateIndex - 1, Context,
      &TransitionDataHolder_t::transition_data,
      init_values...>::StatesPtrArrayHolder_t;
};

template <size_t kFirstStateIndex, typename Context,
          typename Context::StateTransitionArrayPtr_t... init_values>
struct GenerateStatesPtrArrayImpl<kFirstStateIndex, kFirstStateIndex, Context,
                                  init_values...> {
  using TransitionDataHolder_t =
      typename StateTransitions<Context,
                                static_cast<typename Context::State_t>(
                                    kFirstStateIndex)>::TransitionDataHolder_t;

  using StatesPtrArrayHolder_t =
      StatesPtrArrayHolder<Context, &TransitionDataHolder_t::transition_data,
                           init_values...>;
};

template <typename Context>
struct GenerateStatesPtrArray {
  enum : size_t { kFirstState = 0, kLastState = Context::kStateCount - 1 };

  using StatesPtrArrayHolder_t =
      typename GenerateStatesPtrArrayImpl<kFirstState, kLastState,
                                          Context>::StatesPtrArrayHolder_t;
};

//--------------------------------------------
// Enthält Array mit Context::StateTransitionArrayPtr_t pro jeden Zustand und
// für alle Zusände der State-Machine.
template <typename Context>
struct StateMachineDescription {
  // Mit Hilfe von Meta-Function GenerateStatesPtrArray wird
  // StatesPtrArrayHolder-Template-Instanz erzeugt.
  using StatesPtrArrayHolder_t =
      typename GenerateStatesPtrArray<Context>::StatesPtrArrayHolder_t;

  static_assert(static_cast<size_t>(StatesPtrArrayHolder_t::kStatesCount) ==
                static_cast<size_t>(Context::kStateCount));

  static constexpr const TransitionData<Context>& GetTransitionData(
      typename Context::State_t state, typename Context::Event_t event) {
    return StatesPtrArrayHolder_t::GetStateTransitionArray(state)[event];
  }
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

  void DoStartA() {}
  void DoStartB() {}
  void DoEndA() {}
  void DoEndB() {}

  using StateMachine_t = StateMachine<Client1>;
};

#define DEF_TRANS(FSM_DESCR, SRC_STATE, EVENT, DST_STATE, ACTION)             \
  template <>                                                                 \
  struct aofsm::TransitionDescription<FSM_DESCR::Context_t,                   \
                                      FSM_DESCR::State_t::SRC_STATE,          \
                                      FSM_DESCR::Event_t::EVENT> {            \
    static constexpr TransitionData<FSM_DESCR::Context_t> transition_data = { \
        FSM_DESCR::State_t::DST_STATE, &FSM_DESCR::Client_t::ACTION};         \
  };

//----------------------------------------------------------------------------------
//   DEF_TRANS
DEF_TRANS(Client1::StateMachine_t, INITIAL_STATE, kStartAEvt, A_STATE, DoStartA)

DEF_TRANS(Client1::StateMachine_t, INITIAL_STATE, kStartBEvt, B_STATE, DoStartB)

DEF_TRANS(Client1::StateMachine_t, A_STATE, kEndEvt, FINAL_STATE, DoEndA)

DEF_TRANS(Client1::StateMachine_t, B_STATE, kEndEvt, FINAL_STATE, DoEndB)

TEST(aofsm_StateMachineDescription_V2, GetTransitionData_INITIAL_STATE) {
  using Client1Context_t = Client1::StateMachine_t::Context_t;

  using Client1StateMachineDescription_t =
      Client1::StateMachine_t::StateMachineDescription_t;

  using TransitionData_t = Client1::StateMachine_t::TransitionData_t;

  Client1StateMachineDescription_t client_state_machine_description;

  const TransitionData<Client1Context_t>& trans_INITIAL_STATE_kStartAEvt =
      client_state_machine_description.GetTransitionData(Client1::INITIAL_STATE,
                                                         Client1::kStartAEvt);

  EXPECT_EQ(Client1::A_STATE, trans_INITIAL_STATE_kStartAEvt.dest_state);
  EXPECT_EQ(&Client1::DoStartA, trans_INITIAL_STATE_kStartAEvt.action);

  const TransitionData<Client1Context_t>& trans_INITIAL_STATE_kStartBEvt =
      client_state_machine_description.GetTransitionData(Client1::INITIAL_STATE,
                                                         Client1::kStartBEvt);

  EXPECT_EQ(Client1::B_STATE, trans_INITIAL_STATE_kStartBEvt.dest_state);
  EXPECT_EQ(&Client1::DoStartB, trans_INITIAL_STATE_kStartBEvt.action);

  const TransitionData<Client1Context_t>& trans_INITIAL_STATE_kEndEvt =
      client_state_machine_description.GetTransitionData(Client1::INITIAL_STATE,
                                                         Client1::kEndEvt);

  EXPECT_FALSE(Client1::StateMachine_t::IsValidState(
      trans_INITIAL_STATE_kEndEvt.dest_state));
  EXPECT_EQ(nullptr, trans_INITIAL_STATE_kEndEvt.action);
}
