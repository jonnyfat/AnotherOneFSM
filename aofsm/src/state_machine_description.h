// Copyright Yevgen
#ifndef AOFSM_SRC_STATE_MACHINE_DESCRIPTION_H_
#define AOFSM_SRC_STATE_MACHINE_DESCRIPTION_H_

#include "aofsm/src/std_types.h"

#include "aofsm/src/states_array.h"

namespace aofsm {
//--------------------------------------------
// Enthält Array mit Context::StateTransitionArrayPtr_t pro jeden Zustand und
// für alle Zusände der State-Machine.
template <typename Context>
struct StateMachineDescription {
  using StatesArray_t = StatesArray_t<Context>;

  static_assert(static_cast<size_t>(StatesArray_t::kStatesCount) ==
                static_cast<size_t>(Context::kStateCount));

  static constexpr const TransitionData<Context>& GetTransitionData(
      typename Context::State_t state, typename Context::Event_t event) {
    return StatesArray_t::GetStateTransitionArray(state)[event];
  }
};
}  // namespace aofsm

#endif  // AOFSM_SRC_STATE_MACHINE_DESCRIPTION_H_
