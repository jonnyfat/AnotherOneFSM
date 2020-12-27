// Copyright Yevgen
#ifndef AOFSM_SRC_STATE_TRANSITIONS_LIST_H_
#define AOFSM_SRC_STATE_TRANSITIONS_LIST_H_

namespace aofsm {
namespace internal {

// Schlüssel für Suche einer Transition für einen State
// Wird als Parameter für Argument Dependent Lookup benutzt
template <typename Event, Event event>
using EventVaueHolder_t = ValueHolder<Event, event>;

// StateEvents primary template
template <typename State, typename Event, typename Action, State state,
          size_t event_index = Event::kEventCount - 1>
struct StateTransitionsList
    : public StateTransitionsList<State, Event, Action, state,
                                  event_index - 1> {
  using Base_t =
      StateTransitionsList<State, Event, Action, state, event_index - 1>;

  using Base_t::GetTransitionData;

  using EventValue_t =
      EventVaueHolder_t<Event, static_cast<Event>(event_index)>;

  static constexpr TransitionData<State, Action> GetTransitionData(
      const EventValue_t&) {
    using TransitionMapEntry_t =
        TransitionDescription<State, Event, Action, state,
                              static_cast<Event>(event_index)>;
    return TransitionMapEntry_t::transition_data;
  }
};

// StateEvents secondary template
template <typename State, typename Event, typename Action, State state>
struct StateTransitionsList<State, Event, Action, state,
                            static_cast<size_t>(-1)> {
  static constexpr void GetTransitionData() {}
};

}  // namespace internal
}  // namespace aofsm

#endif  // AOFSM_SRC_STATE_TRANSITIONS_LIST_H_
