// copyright Yevgen

#ifndef AOFSM_SRC_STATE_MACHINE_H_
#define AOFSM_SRC_STATE_MACHINE_H_

#include <cstddef>

namespace aofsm {

using std::size_t;

template <typename Client_t>
class StateMachine {
 public:
  using Action_t = void (Client_t::*)(void);

  struct Transition {
    typename Client_t::State src_state;
    typename Client_t::Event event;
    Action_t action;
    typename Client_t::State dst_state;
  };

  struct Configuration {
    size_t transitions_count;
    Transition* transitions_array;
  };

  StateMachine(Client_t* client, const Configuration& configuration);
  virtual ~StateMachine() = default;

 private:
  Client_t* client_{nullptr};
};

}  // namespace aofsm

template <typename Client_t>
inline aofsm::StateMachine<Client_t>::StateMachine(
    Client_t* client, const Configuration& configuration)
    : client_{client} {}

#endif  // AOFSM_SRC_STATE_MACHINE_H_
