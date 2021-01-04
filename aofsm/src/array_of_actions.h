// Copyright Yevgen
#ifndef AOFSM_SRC_ARRAY_OF_ACTIONS_H_
#define AOFSM_SRC_ARRAY_OF_ACTIONS_H_

#include "aofsm/src/std_types.h"

namespace aofsm {
namespace internal {



// Es muss struct sein
template <size_t MAX_ACTIONS_PER_TRANSITION, typename Action_t>
class ArrayOfActions {
 public:
  ArrayOfActions() : action_count{0} {}

  template <class... Actions>
  ArrayOfActions(Action_t first_action, Actions... actions)
      : action_count{1 + sizeof...(actions)},
        action_array{first_action, actions...} {}

  // ist trivial kopierbar
  ArrayOfActions(const ArrayOfActions&) = default;
  ArrayOfActions(ArrayOfActions&&) = default;
  ArrayOfActions& operator=(const ArrayOfActions&) = default;
  ArrayOfActions& operator=(ArrayOfActions&&) = default;

  template <typename Client_t, typename... Params>
  void CallFor(Client_t* client, Params... params) const {
    for (size_t i = 0; i < action_count; ++i) {
      Action_t action = action_array[i];
      (client->*action)(params...);
    }
  }

  void SetEmpty() { action_count = 0; }

  bool IsEmpty() const { return action_count == 0; }

  void SetInvalid() {
    action_count = 1;
    action_array[0] = nullptr;
  }

  bool IsInvalid() const {
    return action_count > 0 && action_array[0] == nullptr;
  }

 private:
  size_t action_count{0};
  Action_t action_array[MAX_ACTIONS_PER_TRANSITION];
};

}  // namespace internal
}  // namespace aofsm

#endif  // AOFSM_SRC_ARRAY_OF_ACTIONS_H_
