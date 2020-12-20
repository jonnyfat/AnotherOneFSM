// copyright Yevgen
//

#include <cstddef>

using std::size_t;

#include "gmock/gmock.h"
#include "gtest/gtest.h"

template <typename Value, Value value>
class ValueHolder {
 public:
  static Value Get() { return value; }
};

template <typename Client_t, typename State_t = typename Client_t::State,
          typename Event_t = typename Client_t::Event,
          typename... ActionParameterTypes>
class StateMachine {
 public:
  // Pointer auf Member-Methode des Clients, welche bei einer Transition als
  // Action aufgerufen wird.
  using Action_t = void (Client_t::*)(ActionParameterTypes...);

  template <State_t state, Event_t event>
  class Transition {
    using DestState_t = ValueHolder<State_t, State_t::kStateCount>;
    using Action_t = ValueHolder<Action_t, nullptr>;
  };

  struct TransitionInfo {
    State_t dest_state;
    Action_t action;
  };

  TransitionInfo GetTransitionInfo() {}
};

template <typename Transition, typename State_t, State_t state,
          typename Event_t>
class StateEvents {};

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

  using StateMachine = StateMachine<Client1>;
};

template <>
template <>
class StateMachine<Client1>::Transition<Client1::INITIAL_STATE,
                                        Client1::kStartAEvt> {
  using DestState_t = ValueHolder<Client1::State, Client1::INITIAL_STATE>;
  using Action_t = ValueHolder<Client1::Action_t, &Client1::DoStartA>;
};

template <Client1::State state>
struct State {
  static constexpr Client1::State GetState() { return state; }
};

template <size_t state_index>
struct StateIndex {
  static constexpr size_t GetValue() { return state_index; }
};

TEST(aofsm_StateMachine, trigger) {
  size_t state_value = Client1::INITIAL_STATE - 1;

  Client1::State state1 = State<Client1::INITIAL_STATE>::GetState();
  Client1::State state2 = State<static_cast<Client1::State>(0)>::GetState();

  size_t state_index1 = StateIndex<>;
}
