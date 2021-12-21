// Copyright Yevgen
#ifndef AOFSM_SRC_TRANSITION_DESCRIPTION_H_
#define AOFSM_SRC_TRANSITION_DESCRIPTION_H_

#include "aofsm/src/state_machine_v2/transition_data.h"

#include "aofsm/src/state_machine_v2/invalid_state.h"

#include "aofsm/src/state_machine_v2/invalid_action.h"

namespace aofsm {

using internal::InvalidAction;
using internal::InvalidState;

// TransitionDescription dient der Definition von Client-State-Machine.
// Eine-Template-Instanz entält Daten einer Transitionen für ein Event in einem
// Zustand.
// Durch Template-Spezialisierung können die State-Machine-Transitionen
// definiert werden.
template <typename Context, typename Context::State_t src_state,
          typename Context::Event_t event>
struct TransitionDescription {
  // Default-Werte für Destination State und Action, falls für src_state und
  // event nicht anderes spezialsiert wurde.
  static constexpr TransitionData<Context> transition_data{
      InvalidState<typename Context::State_t>::value,
      InvalidAction<typename Context::Action_t>::value};
};

}  // namespace aofsm

#endif  // AOFSM_SRC_TRANSITION_DESCRIPTION_H_
