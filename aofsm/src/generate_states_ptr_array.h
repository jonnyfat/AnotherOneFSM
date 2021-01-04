// Copyright Yevgen
#ifndef AOFSM_SRC_GENERATE_STATES_PTR_ARRAY_H_
#define AOFSM_SRC_GENERATE_STATES_PTR_ARRAY_H_

#include "aofsm/src/std_types.h"

#include "aofsm/src/state_transitions.h"

#include "aofsm/src/states_ptr_array_holder.h"

namespace aofsm {

//-----------------------
// GenerateStatesPtrArrayImpl
template <size_t kFirstStateIndex, size_t kLastStateIndex, typename Context,
          typename Context::StateTransitionArrayPtr_t... init_values>
struct GenerateStatesPtrArrayImpl {
  using TransitionDataHolder_t =
      typename StateTransitions<Context,
                                static_cast<typename Context::State_t>(
                                    kLastStateIndex)>::TransitionDataHolder_t;

  using StatesPtrArrayHolder_t = typename GenerateStatesPtrArrayImpl<
      kFirstStateIndex, kLastStateIndex - 1, Context,
      &TransitionDataHolder_t::transition_data,
      init_values...>::StatesPtrArrayHolder_t;
};

template <size_t kFirstStateIndex, typename Context,
          typename Context::StateTransitionArrayPtr_t... init_values>
struct GenerateStatesPtrArrayImpl<kFirstStateIndex, kFirstStateIndex, Context,
                                  init_values...> {
  using TransitionDataHolder_t =
      typename StateTransitions<Context,
                                static_cast<typename Context::State_t>(
                                    kFirstStateIndex)>::TransitionDataHolder_t;

  using StatesPtrArrayHolder_t =
      StatesPtrArrayHolder<Context, &TransitionDataHolder_t::transition_data,
                           init_values...>;
};

template <typename Context>
struct GenerateStatesPtrArray {
  enum : size_t { kFirstState = 0, kLastState = Context::kStateCount - 1 };

  using StatesPtrArrayHolder_t =
      typename GenerateStatesPtrArrayImpl<kFirstState, kLastState,
                                          Context>::StatesPtrArrayHolder_t;
};

}  // namespace aofsm

#endif  // AOFSM_SRC_GENERATE_STATES_PTR_ARRAY_H_
