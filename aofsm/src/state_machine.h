// copyright Yevgen

#ifndef AOFSM_SRC_STATE_MACHINE_H_
#define AOFSM_SRC_STATE_MACHINE_H_

#include <cstddef>

namespace aofsm {

using std::size_t;

// Die Klasse stellt eine State-Machine für einen Client.
// Der Client muss die State-Machine mit folgenden Parametern parametrieren:
// - Anzahl der Zustände
// - Anzahl der Events
// - Transitionen
//
// Template-Parameter:
//  - Client_t - Klasse der Clients
//
// Anforderungen an Client:
//
//  - Client muss folgende nested Typen haben:
//    -  enum State { kInitState, .... , kStateCount};
//    -  enum Event { .... , kEventCount };
//
//  - Client muss folgende Methoden implementieren:
//    - DoUnknownEvent();
//    - DoUnknownTransition();
//
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
    const size_t transitions_count;
    const Transition* transitions_array;
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
