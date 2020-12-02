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
  enum : size_t { MAX_ACTIONS_PER_TRANSITION = 3 };

  using Action_t = void (Client_t::*)(void);
  using State_t = typename Client_t::State;
  using Event_t = typename Client_t::Event;

  struct ArrayOfActions {
    template <class... Actions>
    ArrayOfActions(Actions... actions)
        : action_count{sizeof...(actions)}, action_array{actions...} {}

    size_t action_count;
    Action_t action_array[MAX_ACTIONS_PER_TRANSITION];
  };

  struct Transition {
    State_t src_state;
    Event_t event;
    State_t dst_state;
    ArrayOfActions actions;
  };

  struct DefaultAction {
    Event_t event;
    ArrayOfActions actions;
  };

  struct DefaultTransition {
    Event_t event;
    State_t dst_state;
    ArrayOfActions actions;
  };

  struct Configuration {
    const Transition* transitions;
    size_t transitions_count;
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
