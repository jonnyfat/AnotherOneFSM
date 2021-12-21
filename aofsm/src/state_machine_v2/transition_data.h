// Copyright Yevgen
#ifndef AOFSM_SRC_TRANSITION_DATA_H_
#define AOFSM_SRC_TRANSITION_DATA_H_

#include "aofsm/src/state_machine_v2/invalid_state.h"

namespace aofsm {

using internal::InvalidState;

template <typename Context>
struct TransitionData {
  typename Context::State_t dest_state;
  typename Context::Action_t action;
  bool IsInvalidState() const {
    return dest_state == InvalidState<typename Context::State_t>::value;
  }
  bool IsValidState() const { return !IsInvalidState(); }
};

}  // namespace aofsm

#endif  // AOFSM_SRC_TRANSITION_DATA_H_
