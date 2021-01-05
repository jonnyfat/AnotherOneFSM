// Copyright Yevgen
#ifndef AOFSM_SRC_STATE_TRANSITIONS_GENERATOR_H_
#define AOFSM_SRC_STATE_TRANSITIONS_GENERATOR_H_

#include "aofsm/src/transition_description.h"

#include "aofsm/src/state_transitions_holder.h"

namespace aofsm {

//-----------------------
// StateTransitionsGenerator: definition
template <size_t kFirstEventIndex, size_t kLastEventIndex, typename Context,
          typename Context::State_t state, typename... TransitionDescriptions>
struct StateTransitionsGenerator {
  using TransitionDescription_t = TransitionDescription<
      Context, state, static_cast<typename Context::Event_t>(kLastEventIndex)>;

  using StateTransitionsHolder_t = typename StateTransitionsGenerator<
      kFirstEventIndex, kLastEventIndex - 1, Context, state,
      TransitionDescription_t,
      TransitionDescriptions...>::StateTransitionsHolder_t;
};

// StateTransitionsGenerator: terminal spezialisation
template <size_t kFirstEventIndex, typename Context,
          typename Context::State_t state, typename... TransitionDescriptions>
struct StateTransitionsGenerator<kFirstEventIndex, kFirstEventIndex, Context,
                                 state, TransitionDescriptions...> {
  using TransitionDescription_t = TransitionDescription<
      Context, state, static_cast<typename Context::Event_t>(kFirstEventIndex)>;

  using StateTransitionsHolder_t =
      StateTransitionsHolder<Context, TransitionDescription_t,
                           TransitionDescriptions...>;
};

}  // namespace aofsm

#endif  // AOFSM_SRC_STATE_TRANSITIONS_GENERATOR_H_
