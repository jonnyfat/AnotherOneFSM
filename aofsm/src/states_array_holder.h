// Copyright Yevgen
#ifndef AOFSM_SRC_STATES_PTR_ARRAY_HOLDER_H_
#define AOFSM_SRC_STATES_PTR_ARRAY_HOLDER_H_

#include "aofsm/src/std_types.h"

namespace aofsm {

//-----------------------
// StatesArrayHolder
// - Instantiiert ein Context::StateTransitionsArrayPtr_t-Array.
//
//      -- StateTransitionsArrayPtr_t ist ein Pointer auf
//         Transition-Tabelle eines Zustandes
//         ( TransitionData<Context>[Context::kEventCount] )
//
// - Intialisiert jedes Context::StateTransitionsArrayPtr_t-Element mit
//   entsprechenden Initalwert aus Templateparameter-Pack init_values
template <typename Context,
          typename Context::StateTransitionsArrayPtr_t... init_values>
struct StatesArrayHolder {
  static constexpr typename Context::StateTransitionsArrayPtr_t
      states[sizeof...(init_values)] = {init_values...};

  enum : size_t { kStatesCount = sizeof...(init_values) };

  static constexpr const typename Context::StateTransitionsArray_t&
  GetStateTransitionsArray(typename Context::State_t state) {
    return *states[state];
  }
};

template <typename Context,
          typename Context::StateTransitionsArrayPtr_t... init_values>
constexpr typename Context::StateTransitionsArrayPtr_t StatesArrayHolder<
    Context, init_values...>::states[sizeof...(init_values)];

}  // namespace aofsm

#endif  // AOFSM_SRC_STATES_PTR_ARRAY_HOLDER_H_
