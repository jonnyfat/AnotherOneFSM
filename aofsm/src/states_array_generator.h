// Copyright Yevgen
#ifndef AOFSM_SRC_STATES_ARRAY_GENERATOR_H_
#define AOFSM_SRC_STATES_ARRAY_GENERATOR_H_

#include "aofsm/src/state_transitions.h"

#include "aofsm/src/states_array_holder.h"

namespace aofsm {

//-----------------------
// StatesArrayGenerator: definition
template <size_t kFirstStateIndex, size_t kLastStateIndex, typename Context,
          typename Context::StateTransitionsArrayPtr_t... init_values>
struct StatesArrayGenerator {
  using StateTransitions_t =
      StateTransitions_t<Context, static_cast<typename Context::State_t>(
                                      kLastStateIndex)>;

  using StatesArrayHolder_t =
      typename StatesArrayGenerator<kFirstStateIndex, kLastStateIndex - 1,
                                    Context,
                                    &StateTransitions_t::transition_data,
                                    init_values...>::StatesArrayHolder_t;
};

// StatesArrayGenerator: terminal spezialisation
template <size_t kFirstStateIndex, typename Context,
          typename Context::StateTransitionsArrayPtr_t... init_values>
struct StatesArrayGenerator<kFirstStateIndex, kFirstStateIndex, Context,
                            init_values...> {
  using StateTransitions_t =
      StateTransitions_t<Context, static_cast<typename Context::State_t>(
                                      kFirstStateIndex)>;

  using StatesArrayHolder_t =
      StatesArrayHolder<Context, &StateTransitions_t::transition_data,
                        init_values...>;
};

}  // namespace aofsm

#endif  // AOFSM_SRC_STATES_ARRAY_GENERATOR_H_
