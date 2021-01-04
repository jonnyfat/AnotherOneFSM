// Copyright Yevgen
#ifndef AOFSM_SRC_DEF_TRANS_H_
#define AOFSM_SRC_DEF_TRANS_H_

#include "aofsm/src/transition_description.h"

#define DEF_TRANS(FSM_DESCR, SRC_STATE, EVENT, DST_STATE, ACTION)             \
  template <>                                                                 \
  struct aofsm::TransitionDescription<FSM_DESCR::Context_t,                   \
                                      FSM_DESCR::State_t::SRC_STATE,          \
                                      FSM_DESCR::Event_t::EVENT> {            \
    static constexpr TransitionData<FSM_DESCR::Context_t> transition_data = { \
        FSM_DESCR::State_t::DST_STATE, &FSM_DESCR::Client_t::ACTION};         \
  };

#endif  // AOFSM_SRC_DEF_TRANS_H_
