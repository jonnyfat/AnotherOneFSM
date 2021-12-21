// Copyright Yevgen
#ifndef AOFSM_SRC_GENERATE_TRANSITION_DATA_H_
#define AOFSM_SRC_GENERATE_TRANSITION_DATA_H_

#include "aofsm/src/state_machine_v2/transition_description.h"

#include "aofsm/src/state_machine_v2/transition_data_holder.h"

namespace aofsm {

//-----------------------
// GenerateTransitionDataImpl: definition
template <size_t kFirstEventIndex, size_t kLastEventIndex, typename Context,
          typename Context::State_t state, typename... TransitionDescriptions>
struct GenerateTransitionDataImpl {
  using TransitionDescription_t = TransitionDescription<
      Context, state, static_cast<typename Context::Event_t>(kLastEventIndex)>;

  using TransitionDataHolder_t = typename GenerateTransitionDataImpl<
      kFirstEventIndex, kLastEventIndex - 1, Context, state,
      TransitionDescription_t,
      TransitionDescriptions...>::TransitionDataHolder_t;
};

// GenerateTransitionDataImpl: terminal spezialisation
template <size_t kFirstEventIndex, typename Context,
          typename Context::State_t state, typename... TransitionDescriptions>
struct GenerateTransitionDataImpl<kFirstEventIndex, kFirstEventIndex, Context,
                                  state, TransitionDescriptions...> {
  using TransitionDescription_t = TransitionDescription<
      Context, state, static_cast<typename Context::Event_t>(kFirstEventIndex)>;

  using TransitionDataHolder_t =
      TransitionDataHolder<Context, TransitionDescription_t,
                           TransitionDescriptions...>;
};

// GenerateTransitionData ist Meta-Function
//   Parameter:
//      Context - Kontext der State-Machine
//      state  - Zustand-Id eines Zustandes der State-Machine
//   Rückgabewert ist ein Typ:
//     TransitionDataHolder_t-Template-Instanz für den Zustand state
template <typename Context, typename Context::State_t state>
struct GenerateTransitionData {
  enum : size_t {
    kFirstEventIndex = 0,
    kLastEventIndex = Context::kEventCount - 1
  };

  using TransitionDataHolder_t =
      typename GenerateTransitionDataImpl<kFirstEventIndex, kLastEventIndex,
                                          Context,
                                          state>::TransitionDataHolder_t;
};

}  // namespace aofsm

#endif  // AOFSM_SRC_GENERATE_TRANSITION_DATA_H_
