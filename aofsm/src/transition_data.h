// Copyright Yevgen
#ifndef AOFSM_SRC_TRANSITION_DATA_H_
#define AOFSM_SRC_TRANSITION_DATA_H_

#include "aofsm/src/invalid_state.h"

namespace aofsm {

using internal::InvalidState;

template <typename State, typename Action>
struct TransitionData {
  State dest_state;
  Action action;
  bool IsInvalidState() const {
    return dest_state == InvalidState<State>::value;
  }
  bool IsValidState() const { return !IsInvalidState(); }
};

}  // namespace aofsm

#endif  // AOFSM_SRC_TRANSITION_DATA_H_
