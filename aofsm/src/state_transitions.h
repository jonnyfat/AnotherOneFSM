// Copyright Yevgen
#ifndef AOFSM_SRC_STATE_TRANSITIONS_H_
#define AOFSM_SRC_STATE_TRANSITIONS_H_

#include "aofsm/src/state_transitions_generator.h"

namespace aofsm {

//--------------------------------------------
// Enthält Array mit TransitionData<> pro jeden Event der State-Machine für ein
// Zustand der State-Machine.
template <typename Context, typename Context::State_t state,
          size_t kFirstEventIndex = 0,
          size_t kLastEventIndex = Context::kEventCount - 1>
// Mit Hilfe von Meta-Function GenerateTransitionDataArray wird
// StateTransitionsHolder-Template-Instanz erzeugt.
using StateTransitions_t =
    typename StateTransitionsGenerator<kFirstEventIndex, kLastEventIndex,
                                       Context,
                                       state>::StateTransitionsHolder_t;

}  // namespace aofsm

#endif  // AOFSM_SRC_STATE_TRANSITIONS_H_
