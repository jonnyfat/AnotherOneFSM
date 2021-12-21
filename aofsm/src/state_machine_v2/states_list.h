// Copyright Yevgen
#ifndef AOFSM_SRC_STATES_LIST_H_
#define AOFSM_SRC_STATES_LIST_H_

#include "aofsm/src/state_transitions_list.h"

namespace aofsm {
namespace internal {

template <typename State, State state>
using StateVaueHolder_t = ValueHolder<State, state>;

// StateMachineStates primary template
template <typename Context,
          size_t state_index = Context::State_t::kStateCount - 1>
struct StatesList : public StatesList<Context, state_index - 1> {
  using Base_t = StatesList<Context, state_index - 1>;
  using State_t = typename Context::State_t;
  using Event_t = typename Context::Event_t;

  template <State_t state>
  using EventsData_t = StateTransitionsList<Context, state>;

  using StateEventsData_t =
      StateTransitionsList<Context, static_cast<State_t>(state_index)>;

  using StateValue_t =
      StateVaueHolder_t<State_t, static_cast<State_t>(state_index)>;

  using Base_t::GetStateEventsData;

  static constexpr StateEventsData_t GetStateEventsData(const StateValue_t&) {
    return StateEventsData_t();
  }

  template <State_t state, Event_t event>
  static constexpr TransitionData<Context> GetTransitionData() {
    return GetStateEventsData(StateVaueHolder_t<State_t, state>())
        .GetTransitionData(EventVaueHolder_t<Event_t, event>());
  }
};

// StateMachineStates secondary template
template <typename Context>
struct StatesList<Context, static_cast<size_t>(-1)> {
  static constexpr void GetStateEventsData() {}
};

}  // namespace internal
}  // namespace aofsm

#endif  // AOFSM_SRC_STATES_LIST_H_
