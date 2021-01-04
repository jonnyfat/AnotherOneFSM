// Copyright Yevgen
#ifndef AOFSM_SRC_STATES_PTR_ARRAY_HOLDER_H_
#define AOFSM_SRC_STATES_PTR_ARRAY_HOLDER_H_

#include "aofsm/src/std_types.h"

namespace aofsm {

//-----------------------
// StatesPtrArrayHolder
// - Instantiiert ein Context::StateTransitionArrayPtr_t-Array.
//
//      -- StateTransitionArrayPtr_t ist ein Pointer auf
//         Transition-Tabelle eines Zustandes
//         ( TransitionData<Context>[Context::kEventCount] )
//
// - Intialisiert jedes Context::StateTransitionArrayPtr_t-Element mit
//   entsprechenden Initalwert aus Templateparameter-Pack init_values
template <typename Context,
          typename Context::StateTransitionArrayPtr_t... init_values>
struct StatesPtrArrayHolder {
  static constexpr typename Context::StateTransitionArrayPtr_t
      states[sizeof...(init_values)] = {init_values...};

  enum : size_t { kStatesCount = sizeof...(init_values) };

  static constexpr const typename Context::StateTransitionArray_t&
  GetStateTransitionArray(typename Context::State_t state) {
    return *states[state];
  }
};

template <typename Context,
          typename Context::StateTransitionArrayPtr_t... init_values>
constexpr typename Context::StateTransitionArrayPtr_t StatesPtrArrayHolder<
    Context, init_values...>::states[sizeof...(init_values)];

}  // namespace aofsm

#endif  // AOFSM_SRC_STATES_PTR_ARRAY_HOLDER_H_
