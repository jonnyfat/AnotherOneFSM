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

// ValueHolder macht aus einem Wert einen Datentyp
template <typename State_t>
struct InvalidState {
  static constexpr State_t value = State_t::kStateCount;
};

template <typename State_t>
constexpr State_t InvalidState<State_t>::value;

// Schlüssel für Suche einer Transitionen
template <typename StateMachineDescription_t,
          typename StateMachineDescription_t::State_t src_state,
          typename StateMachineDescription_t::Event_t event>
struct TransitionKey {
  using SrcState_t =
      ValueHolder<typename StateMachineDescription_t::State_t, src_state>;
  using Event_t =
      ValueHolder<typename StateMachineDescription_t::Event_t, event>;
};

// Daten einer Transitionen für ein Event in einem Zustand
template <typename StateMachineDescription_t,
          typename StateMachineDescription_t::State_t src_state,
          typename StateMachineDescription_t::Event_t event>
struct TransitionMapEntry {
  using State_t = typename StateMachineDescription_t::State_t;

  // Invalid StateId
  using DestState_t = ValueHolder<State_t, InvalidState<State_t>::value>;

  // Invalid Action
  using Action_t =
      ValueHolder<typename StateMachineDescription_t::Action_t, nullptr>;
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

// StateEvents primary template
template <typename StateMachine, typename StateMachine::State_t state,
          size_t event_index = StateMachine::Event_t::kEventCount - 1>
struct StateEvents : public StateEvents<StateMachine, state, event_index - 1> {
  using Base_t = StateEvents<StateMachine, state, event_index - 1>;
  using State_t = typename StateMachine::State_t;
  using Event_t = typename StateMachine::Event_t;
  using Action_t = typename StateMachine::Action_t;

  using TransitionKey_t =
      TransitionKey<StateMachine, state, static_cast<Event_t>(event_index)>;

  using TransitionMapEntry_t =
      TransitionMapEntry<StateMachine, state,
                         static_cast<Event_t>(event_index)>;

  using Base_t::GetTransitionData;

  static constexpr TransitionData<State_t, Action_t> GetTransitionData(
      const TransitionKey_t&) {
    return {TransitionMapEntry_t::DestState_t::value,
            TransitionMapEntry_t::Action_t::value};
  }
};

// StateEvents secondary template
template <typename StateMachine, typename StateMachine::State_t state>
struct StateEvents<StateMachine, state, static_cast<size_t>(-1)> {
  static constexpr void GetTransitionData() {}
};

// StateMachineStates primary template
template <typename StateMachine,
          size_t state_index = StateMachine::State_t::kStateCount - 1>
struct StateMachineStates
    : public StateMachineStates<StateMachine, state_index - 1> {
  using State_t = typename StateMachine::State_t;
  using StateEvents_t =
      StateEvents<StateMachine, static_cast<State_t>(state_index)>;
};

// StateMachineStates secondary template
template <typename StateMachine>
struct StateMachineStates<StateMachine, static_cast<size_t>(-1)> {};

template <typename Client, typename State, typename Event, typename Action>
class StateMachineDescription {
 public:
  using Client_t = Client;
  using State_t = State;
  using Event_t = Event;
  using Action_t = Action;

  template <State_t state, Event_t event>
  using TransitionKey_t = TransitionKey<StateMachineDescription, state, event>;

  using TransitionData_t = TransitionData<State_t, Action_t>;

  struct StateTransitionData {
    TransitionData_t event_transitions[Event::kEventCount];
  };

  StateTransitionData transitions[State::kStateCount];
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
      StateMachineDescription<Client_t, State_t, Event_t, Action_t>;

  static constexpr bool IsValidState(State_t state) {
    return StateMachineDescription_t::IsValidState(state);
  }
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

#define DEF_TRANS(STATE_MACHINE, SRC_STATE, EVENT, DST_STATE, ACTION)         \
  template <>                                                                 \
  struct TransitionMapEntry<STATE_MACHINE, STATE_MACHINE::State_t::SRC_STATE, \
                            STATE_MACHINE::Event_t::EVENT> {                  \
    using DestState_t = ValueHolder<STATE_MACHINE::State_t,                   \
                                    STATE_MACHINE::State_t::DST_STATE>;       \
    using Action_t = ValueHolder<STATE_MACHINE ::Action_t,                    \
                                 &STATE_MACHINE::Client_t::ACTION>;           \
  };

// template <typename Client>
// struct StateMachineDescriptionDef {};
//
// DECL_STATE_MACHINE_DESCRIPTION(Client1, Client1Fsm)

DEF_TRANS(Client1::StateMachineDescription_t, INITIAL_STATE, kStartAEvt,
          A_STATE, DoStartA)

DEF_TRANS(Client1::StateMachineDescription_t, INITIAL_STATE, kStartBEvt,
          B_STATE, DoStartB)

TEST(aofsm_StateMachine, StateEventsInstantiation) {
  StateEvents<Client1::StateMachineDescription_t, Client1::INITIAL_STATE>
      state_events;

  auto transition_data1 = state_events.GetTransitionData(
      Client1::StateMachineDescription_t::TransitionKey_t<
          Client1::INITIAL_STATE, Client1::kStartAEvt>());

  EXPECT_TRUE(transition_data1.IsValidState());
  EXPECT_EQ(transition_data1.dest_state, Client1::A_STATE);
  EXPECT_EQ(transition_data1.action, &Client1::DoStartA);

  auto transition_data2 = state_events.GetTransitionData(
      Client1::StateMachineDescription_t::TransitionKey_t<
          Client1::INITIAL_STATE, Client1::kStartBEvt>());

  EXPECT_TRUE(transition_data2.IsValidState());
  EXPECT_EQ(transition_data2.dest_state, Client1::B_STATE);
  EXPECT_EQ(transition_data2.action, &Client1::DoStartB);

  auto transition_data3 = state_events.GetTransitionData(
      Client1::StateMachineDescription_t::TransitionKey_t<
          Client1::INITIAL_STATE, Client1::kEndEvt>());

  EXPECT_FALSE(transition_data3.IsValidState());
  EXPECT_EQ(transition_data3.action, nullptr);
}

TEST(aofsm_StateMachine, Dummy) {}
