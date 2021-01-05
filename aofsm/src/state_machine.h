// Copyright Yevgen
#ifndef AOFSM_SRC_STATE_MACHINE_H_
#define AOFSM_SRC_STATE_MACHINE_H_

#include "aofsm/src/state_machine_context.h"

#include "aofsm/src/state_machine_description.h"

namespace aofsm {

template <typename Client, typename State = typename Client::State,
          typename Event = typename Client::Event,
          typename... ActionParameterTypes>
class StateMachine {
 public:
  using Client_t = Client;
  using State_t = State;
  using Event_t = Event;

  // Pointer auf Member-Methode des Clients, welche bei einer Transition als
  // Action aufgerufen wird.
  using Action_t = void (Client::*)(ActionParameterTypes...);
  using Guard_t = bool (Client::*)(ActionParameterTypes...);

  using Context_t =
      StateMachineContext<Client, State_t, Event_t, Action_t, Guard_t,
                          State_t::kStateCount, Event_t::kEventCount>;

  using StateMachineDescription_t = StateMachineDescription<Context_t>;

  static constexpr bool IsValidState(State_t state) {
    return state >= 0 && state < State_t::kStateCount;
  }

  State_t current_state_{State_t::INITIAL_STATE};
};

}  // namespace aofsm

#endif  // AOFSM_SRC_STATE_MACHINE_H_
