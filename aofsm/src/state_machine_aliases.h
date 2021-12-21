// Copyright Yevgen
#ifndef AOFSM_SRC_STATE_MACHINE_ALIASES_H_
#define AOFSM_SRC_STATE_MACHINE_ALIASES_H_

#include "aofsm/src/state_machine_v1/state_machine.h"

#define DECL_STATE_MACHINE_WITH_MULT_ACTIONS(__class_name__, __member_name__,  \
                                             __action_count__)                 \
  using StateMachine_t =                                                       \
      aofsm::v1::StateMachineWithCustomActions<__class_name__,                 \
                                               __action_count__>;              \
  using StateMachineDescription_t = StateMachine_t::StateMachineDescription_t; \
  static const StateMachineDescription_t __member_name__##description;         \
  StateMachine_t __member_name__{this, __member_name__##description};

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
