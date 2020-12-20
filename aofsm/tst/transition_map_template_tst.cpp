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

// Schlüssel für Suche einer Transitionen
template <typename StateMachine_t, typename StateMachine_t::State_t src_state,
          typename StateMachine_t::Event_t event>
struct TransitionKey {
  using SrcState_t = ValueHolder<typename StateMachine_t::State_t, src_state>;
  using Event_t = ValueHolder<typename StateMachine_t::Event_t, event>;
};

// Daten einer Transitionen für ein Event in einem Zustand
template <typename StateMachine_t, typename StateMachine_t::State_t src_state,
          typename StateMachine_t::Event_t event>
struct TransitionMapEntry {
  // Invalid StateId
  using DestState_t = ValueHolder<typename StateMachine_t::State_t,
                                  StateMachine_t::kInvalidStateId>;
  // Invalid Action
  using Action_t = ValueHolder<typename StateMachine_t::Action_t, nullptr>;
};

template <typename State, typename Action>
struct TransitionData {
  State dest_state;
  Action transition_action;
};

// primary template
template <typename StateMachine, typename StateMachine::State_t state,
          size_t event_index = StateMachine::kEventCount - 1>
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

template <typename StateMachine, typename StateMachine::State_t state>
struct StateEvents<StateMachine, state, static_cast<size_t>(-1)> {
  static constexpr void GetState() {}
  static constexpr void GetAction() {}
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

  // Konstante für ungültiger StateId
  static constexpr State_t kInvalidStateId = State_t::kStateCount;

  // Konstante für Anzahl der Events
  static constexpr size_t kEventCount = Event_t::kEventCount;

  template <State_t state, Event_t event>
  using TransitionKey_t = TransitionKey<StateMachine, state, event>;

  using TransitionData_t = TransitionData<State_t, Action_t>;
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
};

template <>
struct TransitionMapEntry<Client1::StateMachine_t, Client1::INITIAL_STATE,
                          Client1::kStartAEvt> {
  using DestState_t = ValueHolder<Client1::State, Client1::A_STATE>;
  using Action_t = ValueHolder<Client1::Action_t, &Client1::DoStartA>;
};

template <Client1::State state>
struct State {
  static constexpr Client1::State GetState() { return state; }
};

template <size_t state_index>
struct StateIndex {
  static constexpr size_t GetIndex() { return state_index; }
};

TEST(aofsm_StateMachine, trigger) {
  size_t state_value = Client1::INITIAL_STATE - 1;

  Client1::State state1 = State<Client1::INITIAL_STATE>::GetState();
  Client1::State state2 = State<static_cast<Client1::State>(0)>::GetState();
  //  Client1::State state2 = State<0>::GetState();

  size_t state_index1 =
      StateIndex<static_cast<size_t>(Client1::INITIAL_STATE)>::GetIndex();
  //  size_t state_index1 = StateIndex<Client1::INITIAL_STATE>::GetIndex();
}

TEST(aofsm_StateMachine, StateEventsInstantiation) {
  StateEvents<Client1::StateMachine_t, Client1::INITIAL_STATE> state_events;

  auto state = state_events.GetState(
      Client1::StateMachine_t::TransitionKey_t<Client1::INITIAL_STATE,
                                               Client1::kStartAEvt>());
  auto action = state_events.GetAction(
      Client1::StateMachine_t::TransitionKey_t<Client1::INITIAL_STATE,
                                               Client1::kStartAEvt>());
}
