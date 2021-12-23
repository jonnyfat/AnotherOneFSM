//----------------------------------------------------------------------------
// Copyright: HIMA - Paul Hildebrandt GmbH
//
//----------------------------------------------------------------------------

#ifndef AOFSM_SRC_INTERNAL_STATE_MACHINE_TRANSITION_DATA_H_
#define AOFSM_SRC_INTERNAL_STATE_MACHINE_TRANSITION_DATA_H_

namespace aofsm {

/// StateMachineTransitionData kapselt die Teilinformation einer Transition für
/// einen State und Event.
template <typename Context>
struct StateMachineTransitionData {
  using State_t = typename Context::State_t;
  using Action_t = typename Context::Action_t;
  using Guard_t = typename Context::Guard_t;

  Guard_t guard_action;  ///< Bestimmt ob trans1 oder Trans2 ausgeführt wird
                         ///< trans1 : Transition, wenn guard_action nullptr
                         ///< oder liefert true

  // trans1 : Transition, wenn guard_action nullptr oder liefert true
  State_t trans1_dst_state;
  Action_t trans1_action;

  // trans2 : Transition, wenn guard_action liefert false
  State_t trans2_dst_state;
  Action_t trans2_action;
};

}  // namespace aofsm

#endif  // AOFSM_SRC_INTERNAL_STATE_MACHINE_TRANSITION_DATA_H_
