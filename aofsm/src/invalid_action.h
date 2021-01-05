// Copyright Yevgen
#ifndef AOFSM_SRC_INVALID_ACTION_H_
#define AOFSM_SRC_INVALID_ACTION_H_

namespace aofsm {
namespace internal {

// Type-Trait für einen Action-Type.
// Definiert ungültigen Wert
template <typename Action>
struct InvalidAction {
  static constexpr Action value = nullptr;
};

template <typename Action>
constexpr Action InvalidAction<Action>::value;

}  // namespace internal
}  // namespace aofsm

#endif  // AOFSM_SRC_INVALID_ACTION_H_
