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
template <typename Context, typename Context::State_t state,
          size_t event_index = Context::kEventCount - 1>
struct StateTransitionsList
    : public StateTransitionsList<Context, state, event_index - 1> {
  using Base_t = StateTransitionsList<Context, state, event_index - 1>;

  using Base_t::GetTransitionData;

  using EventValue_t =
      EventVaueHolder_t<typename Context::Event_t,
                        static_cast<typename Context::Event_t>(event_index)>;

  static constexpr TransitionData<Context> GetTransitionData(
      const EventValue_t&) {
    using TransitionMapEntry_t = TransitionDescription<
        Context, state, static_cast<typename Context::Event_t>(event_index)>;
    return TransitionMapEntry_t::transition_data;
  }
};

// StateEvents secondary template
template <typename Context, typename Context::State_t state>
struct StateTransitionsList<Context, state, static_cast<size_t>(-1)> {
  static constexpr void GetTransitionData() {}
};

}  // namespace internal
}  // namespace aofsm

#endif  // AOFSM_SRC_STATE_TRANSITIONS_LIST_H_
