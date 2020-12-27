// Copyright Yevgen
#ifndef AOFSM_SRC_STATES_LIST_H_
#define AOFSM_SRC_STATES_LIST_H_

#include "aofsm/src/state_transitions_list.h"

namespace aofsm {
namespace internal {

template <typename State, State state>
using StateVaueHolder_t = ValueHolder<State, state>;

// StateMachineStates primary template
template <typename State, typename Event, typename Action,
          size_t state_index = State::kStateCount - 1>
struct StatesList : public StatesList<State, Event, Action, state_index - 1> {
  using Base_t = StatesList<State, Event, Action, state_index - 1>;

  template <State state>
  using EventsData_t = StateTransitionsList<State, Event, Action, state>;

  using StateEventsData_t =
      StateTransitionsList<State, Event, Action,
                           static_cast<State>(state_index)>;

  using StateValue_t =
      StateVaueHolder_t<State, static_cast<State>(state_index)>;

  using Base_t::GetStateEventsData;

  static constexpr StateEventsData_t GetStateEventsData(const StateValue_t&) {
    return StateEventsData_t();
  }

  template <State state, Event event>
  static constexpr TransitionData<State, Action> GetTransitionData() {
    return GetStateEventsData(StateVaueHolder_t<State, state>())
        .GetTransitionData(EventVaueHolder_t<Event, event>());
  }
};

// StateMachineStates secondary template
template <typename State, typename Event, typename Action>
struct StatesList<State, Event, Action, static_cast<size_t>(-1)> {
  static constexpr void GetStateEventsData() {}
};

}  // namespace internal
}  // namespace aofsm

#endif  // AOFSM_SRC_STATES_LIST_H_
