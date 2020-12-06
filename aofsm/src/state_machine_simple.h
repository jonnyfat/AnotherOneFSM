// Copyright Yevgen
#ifndef AOFSM_SRC_STATE_MACHINE_SIMPLE_H_
#define AOFSM_SRC_STATE_MACHINE_SIMPLE_H_

#include "aofsm/src/state_machine.h"

namespace aofsm {

template <typename Client_t, size_t MAX_ACTIONS_PER_TRANSITION = 2,
          typename... ActionParameterTypes>
using StateMachineSimple =
    StateMachine<Client_t, typename Client_t::State, typename Client_t::Event,
                 MAX_ACTIONS_PER_TRANSITION, ActionParameterTypes...>;

};

#endif  // AOFSM_SRC_STATE_MACHINE_SIMPLE_H_
