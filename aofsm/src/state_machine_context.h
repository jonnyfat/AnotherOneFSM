// Copyright Yevgen
#ifndef AOFSM_SRC_STATE_MACHINE_CONTEXT_H_
#define AOFSM_SRC_STATE_MACHINE_CONTEXT_H_

#include "aofsm/src/std_types.h"

#include "aofsm/src/transition_data.h"

namespace aofsm {

// Enthält alle für State-Machine Relevanten Typen und Konstanten
template <typename Client, typename State, typename Event, typename Action,
          typename Guard, size_t STATE_COUNT, size_t EVENT_COUNT>
struct StateMachineContext {
  using Client_t = Client;
  using State_t = State;
  using Event_t = Event;
  using Action_t = Action;
  using Guard_t = Guard;

  enum : size_t {
    kStateCount = STATE_COUNT,
    kEventCount = EVENT_COUNT,
  };

  // Konstante für ungültiger StateId
  static constexpr State kInvalidStateId = static_cast<State>(kStateCount);

  using TransitionData_t = TransitionData<StateMachineContext>;

  using StateTransitionsArray_t = TransitionData_t[kEventCount];

  using StateTransitionsArrayPtr_t = StateTransitionsArray_t const *;
};

}  // namespace aofsm

#endif  // AOFSM_SRC_STATE_MACHINE_CONTEXT_H_
