// Copyright Yevgen
#ifndef AOFSM_SRC_STATE_MACHINE_ALIASES_H_
#define AOFSM_SRC_STATE_MACHINE_ALIASES_H_

#include "aofsm/src/state_machine_v1/state_machine.h"

namespace aofsm {
namespace v1 {
template <typename Client_t, size_t MAX_ACTIONS_PER_TRANSITION,
          typename... ActionParameterTypes>
using StateMachineWithCustomActions =
    StateMachine<Client_t, typename Client_t::State, typename Client_t::Event,
                 MAX_ACTIONS_PER_TRANSITION, ActionParameterTypes...>;
}
};  // namespace aofsm

#endif  // AOFSM_SRC_STATE_MACHINE_ALIASES_H_
