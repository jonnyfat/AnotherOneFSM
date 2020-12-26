// copyright Yevgen
//

#include <cstddef>

using std::size_t;

#include "gmock/gmock.h"
#include "gtest/gtest.h"

// ValueHolder macht aus einem Wert einen Datentyp
template <typename ValueType, ValueType VALUE>
struct ValueHolder {
  using ValueType_t = ValueType;
  static constexpr ValueType value = VALUE;
};

// "Generiert" für einen State-Type einen ungpltigen Wert
template <typename State_t>
struct InvalidState {
  static constexpr State_t value = State_t::kStateCount;
};
// Falls dem Linker InvalidState<>::value fehlt, dann muss folgendes
// auskommentiert werden template <typename State_t> constexpr State_t
// InvalidState<State_t>::value;

// Daten einer Transitionen für ein Event in einem Zustand
template <typename State, typename Event, typename Action, State src_state,
          Event event>
struct TransitionMapEntry {
  // Invalid StateId
  using DestState_t = ValueHolder<State, InvalidState<State>::value>;

  // Invalid Action
  using Action_t = ValueHolder<Action, nullptr>;
};

template <typename State, typename Action>
struct TransitionData {
  State dest_state;
  Action action;
  bool IsInvalidState() const {
    return dest_state == InvalidState<State>::value;
  }
  bool IsValidState() const { return !IsInvalidState(); }
};

// Schlüssel für Suche einer Transition für einen State
// Wird als Parameter für Argument Dependent Lookup benutzt
template <typename Event, Event event>
using EventKeyVaueHolder_t = ValueHolder<Event, event>;

// StateEvents primary template
template <typename State, typename Event, typename Action, State state,
          size_t event_index = Event::kEventCount - 1>
struct StateEvents
    : public StateEvents<State, Event, Action, state, event_index - 1> {
  using Base_t = StateEvents<State, Event, Action, state, event_index - 1>;
  using State_t = State;
  using Event_t = Event;
  using Action_t = Action;

  using EventKeyKey_t =
      EventKeyVaueHolder_t<Event, static_cast<Event_t>(event_index)>;

  using TransitionMapEntry_t =
      TransitionMapEntry<State, Event, Action, state,
                         static_cast<Event_t>(event_index)>;

  using Base_t::GetTransitionData;

  static constexpr TransitionData<State_t, Action_t> GetTransitionData(
      const EventKeyKey_t&) {
    return {TransitionMapEntry_t::DestState_t::value,
            TransitionMapEntry_t::Action_t::value};
  }
};

// StateEvents secondary template
template <typename State, typename Event, typename Action, State state>
struct StateEvents<State, Event, Action, state, static_cast<size_t>(-1)> {
  static constexpr void GetTransitionData() {}
};

// StateMachineStates primary template
template <typename State, typename Event, typename Action,
          size_t state_index = State::kStateCount - 1>
struct StateMachineStates
    : public StateMachineStates<State, Event, Action, state_index - 1> {
  using Base_t = StateMachineStates<State, Event, Action, state_index - 1>;

  template <State state>
  using EventsData_t = StateEvents<State, Event, Action, state>;

  using StateEventsData_t =
      StateEvents<State, Event, Action, static_cast<State>(state_index)>;

  template <State state>
  using StateKeyVaueHolder_t = ValueHolder<State, state>;

  using StateKey_t = StateKeyVaueHolder_t<static_cast<State>(state_index)>;

  using Base_t::GetStateEventsData;

  static constexpr StateEventsData_t GetStateEventsData(const StateKey_t&) {
    return StateEventsData_t();
  }

  template <Event event>
  using EventKeyVaueHolder_t = EventKeyVaueHolder_t<Event, event>;

  template <State state, Event event>
  static constexpr TransitionData<State, Action> GetTransitionData() {
    return GetStateEventsData(StateKeyVaueHolder_t<state>())
        .GetTransitionData(EventKeyVaueHolder_t<event>());
  }
};

// StateMachineStates secondary template
template <typename State, typename Event, typename Action>
struct StateMachineStates<State, Event, Action, static_cast<size_t>(-1)> {
  static constexpr void GetStateEventsData() {}
};

template <typename State, typename Event, typename Action>
class StateMachineDescription {
 public:
  using State_t = State;
  using Event_t = Event;
  using Action_t = Action;

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

  template <State state>
  using StateEvents_t = StateEvents<State_t, Event_t, Action_t, state>;

  using StateMachineStates_t = StateMachineStates<State_t, Event_t, Action_t>;

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

#define DEF_TRANS(STATE_MACHINE, SRC_STATE, EVENT, DST_STATE, ACTION)          \
  template <>                                                                  \
  struct TransitionMapEntry<                                                   \
      STATE_MACHINE::State_t, STATE_MACHINE::Event_t, STATE_MACHINE::Action_t, \
      STATE_MACHINE::State_t::SRC_STATE, STATE_MACHINE::Event_t::EVENT> {      \
    using DestState_t = ValueHolder<STATE_MACHINE::State_t,                    \
                                    STATE_MACHINE::State_t::DST_STATE>;        \
    using Action_t = ValueHolder<STATE_MACHINE ::Action_t,                     \
                                 &STATE_MACHINE::Client_t::ACTION>;            \
  };

DEF_TRANS(Client1::StateMachine_t, INITIAL_STATE, kStartAEvt, A_STATE, DoStartA)

DEF_TRANS(Client1::StateMachine_t, INITIAL_STATE, kStartBEvt, B_STATE, DoStartB)

TEST(aofsm_StateMachine, StateEventsInstantiation) {
  Client1::StateMachine_t::StateMachineStates_t state_machine_states;

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
