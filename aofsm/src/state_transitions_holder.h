// Copyright Yevgen
#ifndef AOFSM_SRC_TRANSITION_DATA_HOLDER_H_
#define AOFSM_SRC_TRANSITION_DATA_HOLDER_H_

#include "aofsm/src/transition_data.h"

namespace aofsm {

//-----------------------
// StateTransitionsHolder
// - instantiiert ein TransitionData<>-Array
// - intialisiert jedes TransitionData<>-Element mit
// TransitionDescription, welche als Template-Parameter angegeben werden muss
template <typename Context, typename... TransitionDescriptions>
struct StateTransitionsHolder {
  // Array mit Transition-Informationen
  static constexpr TransitionData<Context> transition_data[sizeof...(
      TransitionDescriptions)] = {{TransitionDescriptions::transition_data}...};
  // Anzahl der Elemente in Array transition_data
  enum : size_t { kTransitionDataCount = sizeof...(TransitionDescriptions) };
};

template <typename Context, typename... TransitionDescriptions>
constexpr TransitionData<Context>
    StateTransitionsHolder<Context, TransitionDescriptions...>::transition_data
        [sizeof...(TransitionDescriptions)];

}  // namespace aofsm

#endif  // AOFSM_SRC_TRANSITION_DATA_HOLDER_H_
