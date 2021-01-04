// Copyright Yevgen
#ifndef AOFSM_SRC_DEF_TRANS_H_
#define AOFSM_SRC_DEF_TRANS_H_

#include "aofsm/src/transition_description.h"

// Deklaration von Default-Action für ein Event
// Die State-Machine bleibt in gleichem Zustand
#define DECL_DEFAULT_ACTION(FSM_DESCR, EVENT, ACTION)                         \
  template <typename Context, typename Context::State_t src_state>            \
  struct aofsm::TransitionDescription<FSM_DESCR::Context_t, src_state,        \
                                      FSM_DESCR::Event_t::EVENT> {            \
    static constexpr TransitionData<FSM_DESCR::Context_t> transition_data = { \
        src_state, &FSM_DESCR::Client_t::ACTION};                             \
  };

// Deklaration von Default-Transition für ein Event für beliebigen Zustand
#define DECL_DEF_TRANS(FSM_DESCR, EVENT, DST_STATE, ACTION)                   \
  template <typename Context, typename Context::State_t src_state>            \
  struct aofsm::TransitionDescription<FSM_DESCR::Context_t, src_state,        \
                                      FSM_DESCR::Event_t::EVENT> {            \
    static constexpr TransitionData<FSM_DESCR::Context_t> transition_data = { \
        FSM_DESCR::State_t::DST_STATE, &FSM_DESCR::Client_t::ACTION};         \
  };

// Deklaration von Transition für ein Event in einem Zustand
#define DECL_TRANS(FSM_DESCR, SRC_STATE, EVENT, DST_STATE, ACTION)            \
  template <>                                                                 \
  struct aofsm::TransitionDescription<FSM_DESCR::Context_t,                   \
                                      FSM_DESCR::State_t::SRC_STATE,          \
                                      FSM_DESCR::Event_t::EVENT> {            \
    static constexpr TransitionData<FSM_DESCR::Context_t> transition_data = { \
        FSM_DESCR::State_t::DST_STATE, &FSM_DESCR::Client_t::ACTION};         \
  };

#endif  // AOFSM_SRC_DEF_TRANS_H_
