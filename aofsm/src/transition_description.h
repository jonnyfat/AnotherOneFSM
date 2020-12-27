// Copyright Yevgen
#ifndef AOFSM_SRC_TRANSITION_DESCRIPTION_H_
#define AOFSM_SRC_TRANSITION_DESCRIPTION_H_

#include "aofsm/src/transition_data.h"

#include "aofsm/src/invalid_state.h"

#include "aofsm/src/invalid_action.h"

namespace aofsm {

using internal::InvalidAction;
using internal::InvalidState;

// Daten einer Transitionen für ein Event in einem Zustand
// Durch Template-Spezialisierung können die State-Machine-Transitionen zur
// Kompilierzeit berechnet werden.
template <typename State, typename Event, typename Action, State src_state,
          Event event>
struct TransitionDescription {
  // Default-Werte für Destination State und Action, falls für src_state und
  // event nicht anderes spezialsiert wurde.
  static constexpr TransitionData<State, Action> transition_data{
      InvalidState<State>::value, InvalidAction<Action>::value};
};

}  // namespace aofsm

#endif  // AOFSM_SRC_TRANSITION_DESCRIPTION_H_
