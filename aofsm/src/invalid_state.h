// Copyright Yevgen
#ifndef AOFSM_SRC_INVALID_STATE_H_
#define AOFSM_SRC_INVALID_STATE_H_

namespace aofsm {
namespace internal {

// Type-Trait für einen State-Type.
// Definiert ungültigen Wert
template <typename State>
struct InvalidState {
  static constexpr State value = State::kStateCount;
};

template <typename State>
constexpr State InvalidState<State>::value;

}  // namespace internal
}  // namespace aofsm

#endif  // AOFSM_SRC_INVALID_STATE_H_
