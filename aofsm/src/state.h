// Copyright Yevgen
#ifndef AOFSM_SRC_STATE_H_
#define AOFSM_SRC_STATE_H_

namespace aofsm {

template <typename T>
struct State {
  bool is_valid;
  T value;

  State() : is_valid{false}, value{} {}

  State(T state) : is_valid{true}, value{state} {}
};

}  // namespace aofsm

#endif  // AOFSM_SRC_STATE_H_
