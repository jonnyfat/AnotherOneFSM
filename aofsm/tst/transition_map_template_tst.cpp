// copyright Yevgen
//

#include <cstddef>

using std::size_t;

#include "gmock/gmock.h"
#include "gtest/gtest.h"

// ValueHolder macht aus einem Wert einen Datentyp
template <typename Value, Value VALUE>
struct ValueHolder {
  static constexpr Value value = VALUE;
};

// ValueHolder macht aus einem Wert einen Datentyp
template <typename State_t>
struct InvalidState {
  static constexpr State_t value = State_t::kStateCount;
};

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
  Action transition_action;
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

  using Base_t::GetAction;
  using Base_t::GetState;

  static constexpr State_t GetState(const TransitionKey_t&) {
    return TransitionMapEntry_t::DestState_t::value;
  }
  static constexpr Action_t GetAction(const TransitionKey_t&) {
    return TransitionMapEntry_t::Action_t::value;
  }
};

// StateEvents secondary template
template <typename StateMachine, typename StateMachine::State_t state>
struct StateEvents<StateMachine, state, static_cast<size_t>(-1)> {
  static constexpr void GetState() {}
  static constexpr void GetAction() {}
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

template <typename State, typename Event, typename Action>
class StateMachineDescription {
 public:
  using State_t = State;
  using Event_t = Event;
  using Action_t = Action;

  template <State_t state, Event_t event>
  using TransitionKey_t = TransitionKey<StateMachineDescription, state, event>;

  using TransitionData_t = TransitionData<State_t, Action_t>;

  static constexpr bool IsValidState(State_t state) {
    return state >= 0 && state < State_t::kStateCount;
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

  using StateMachineDescription_t =
      StateMachineDescription<State_t, Event_t, Action_t>;

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
    using Action_t = ValueHolder<STATE_MACHINE ::Action_t, &ACTION>;          \
  };

DEF_TRANS(Client1::StateMachineDescription_t, INITIAL_STATE, kStartAEvt,
          A_STATE, Client1::DoStartA)

DEF_TRANS(Client1::StateMachineDescription_t, INITIAL_STATE, kStartBEvt,
          B_STATE, Client1::DoStartB)

TEST(aofsm_StateMachine, StateEventsInstantiation) {
  StateEvents<Client1::StateMachineDescription_t, Client1::INITIAL_STATE>
      state_events;

  auto state1 =
      state_events.GetState(Client1::StateMachineDescription_t::TransitionKey_t<
                            Client1::INITIAL_STATE, Client1::kStartAEvt>());
  auto action1 = state_events.GetAction(
      Client1::StateMachineDescription_t::TransitionKey_t<
          Client1::INITIAL_STATE, Client1::kStartAEvt>());

  EXPECT_TRUE(Client1::StateMachineDescription_t::IsValidState(state1));
  EXPECT_EQ(state1, Client1::A_STATE);
  EXPECT_EQ(action1, &Client1::DoStartA);

  auto state2 =
      state_events.GetState(Client1::StateMachineDescription_t::TransitionKey_t<
                            Client1::INITIAL_STATE, Client1::kStartBEvt>());
  auto action2 = state_events.GetAction(
      Client1::StateMachineDescription_t::TransitionKey_t<
          Client1::INITIAL_STATE, Client1::kStartBEvt>());

  EXPECT_TRUE(Client1::StateMachineDescription_t::IsValidState(state2));
  EXPECT_EQ(state2, Client1::B_STATE);
  EXPECT_EQ(action2, &Client1::DoStartB);

  auto state3 =
      state_events.GetState(Client1::StateMachineDescription_t::TransitionKey_t<
                            Client1::INITIAL_STATE, Client1::kEndEvt>());
  auto action3 = state_events.GetAction(
      Client1::StateMachineDescription_t::TransitionKey_t<
          Client1::INITIAL_STATE, Client1::kEndEvt>());

  EXPECT_FALSE(Client1::StateMachineDescription_t::IsValidState(state3));
  EXPECT_EQ(action3, nullptr);
}
