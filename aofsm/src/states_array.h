// Copyright Yevgen
#ifndef AOFSM_SRC_STATES_ARRAY_H_
#define AOFSM_SRC_STATES_ARRAY_H_

#include "aofsm/src/std_types.h"

#include "aofsm/src/states_array_generator.h"

namespace aofsm {

template <typename Context, size_t kFirstState = 0,
          size_t kLastState = Context::kStateCount - 1>
using StatesArray_t =
    typename StatesArrayGenerator<kFirstState, kLastState,
                                  Context>::StatesArrayHolder_t;

}  // namespace aofsm

#endif  // AOFSM_SRC_STATES_ARRAY_H_
