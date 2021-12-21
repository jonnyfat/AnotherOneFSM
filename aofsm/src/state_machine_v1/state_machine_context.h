// Copyright Yevgen
#ifndef AOFSM_SRC_STATE_MACHINE_CONTEXT_H_
#define AOFSM_SRC_STATE_MACHINE_CONTEXT_H_

namespace aofsm {

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
  static constexpr State kInvalidStateId = State::kStateCount;
};

}  // namespace aofsm

#endif  // AOFSM_SRC_STATE_MACHINE_CONTEXT_H_
