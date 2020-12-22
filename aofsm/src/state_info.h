// Copyright Yevgen
#ifndef AOFSM_SRC_STATE_INFO_H_
#define AOFSM_SRC_STATE_INFO_H_

#include <cstddef>

namespace aofsm {

using std::size_t;

template <typename State_t, typename Action_t>
struct StateInfo {
  const State_t state;

  const Action_t& on_entry_action;

  const Action_t& on_exit_action;

  const size_t sub_states_count{0};

  const State_t sub_states[State_t::kStateCount];

  template <class... SubStates>
  StateInfo(State_t state, const Action_t& on_entry_action,
            const Action_t& on_exit_action, SubStates... sub_states)
      : state{state},
        on_entry_action{on_entry_action},
        on_exit_action{on_exit_action},
        sub_states_count{sizeof...(sub_states)},
        sub_states{sub_states...} {}
};

}  // namespace aofsm

#endif  // AOFSM_SRC_STATE_INFO_H_
