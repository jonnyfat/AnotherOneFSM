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
// - Default-Transitionen (optional)
// - Default-Actions (optional)
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

  struct DefaultTransition {
    Event_t event;
    State_t dst_state;
    ArrayOfActions actions;
  };

  struct DefaultAction {
    Event_t event;
    ArrayOfActions actions;
  };

  template <size_t N>
  using TransitionArray = Transition[N];

  template <size_t N>
  using DefaultTransitionArray = DefaultTransition[N];

  template <size_t N>
  using DefaultActionArray = DefaultAction[N];

  template <size_t TRANSITION_COUNT>
  StateMachine(Client_t* client,
               const TransitionArray<TRANSITION_COUNT>& transitions);

  template <size_t TRANSITION_COUNT, size_t DEFAULT_TRANSITION_COUNT>
  StateMachine(Client_t* client,
               const TransitionArray<TRANSITION_COUNT>& transitions,
               const DefaultTransitionArray<DEFAULT_TRANSITION_COUNT>&
                   default_transitions);

  template <size_t TRANSITION_COUNT, size_t DEFAULT_ACTION_COUNT>
  StateMachine(Client_t* client,
               const TransitionArray<TRANSITION_COUNT>& transitions,
               const DefaultActionArray<DEFAULT_ACTION_COUNT>& default_actions);

  template <size_t TRANSITION_COUNT, size_t DEFAULT_TRANSITION_COUNT,
            size_t DEFAULT_ACTION_COUNT>
  StateMachine(Client_t* client,
               const TransitionArray<TRANSITION_COUNT>& transitions,
               const DefaultTransitionArray<DEFAULT_TRANSITION_COUNT>&
                   default_transitions,
               const DefaultActionArray<DEFAULT_ACTION_COUNT>& default_actions);

  virtual ~StateMachine() = default;

 private:
  Client_t* client_{nullptr};
};

template <typename Client_t>
template <size_t TRANSITION_COUNT>
StateMachine<Client_t>::StateMachine(
    Client_t* client, const TransitionArray<TRANSITION_COUNT>& transitions)
    : client_{client} {}

template <typename Client_t>
template <size_t TRANSITION_COUNT, size_t DEFAULT_TRANSITION_COUNT>
StateMachine<Client_t>::StateMachine(
    Client_t* client, const TransitionArray<TRANSITION_COUNT>& transitions,
    const DefaultTransitionArray<DEFAULT_TRANSITION_COUNT>& default_transitions)
    : client_{client} {}

template <typename Client_t>
template <size_t TRANSITION_COUNT, size_t DEFAULT_ACTION_COUNT>
StateMachine<Client_t>::StateMachine(
    Client_t* client, const TransitionArray<TRANSITION_COUNT>& transitions,
    const DefaultActionArray<DEFAULT_ACTION_COUNT>& default_actions)
    : client_{client} {}

template <typename Client_t>
template <size_t TRANSITION_COUNT, size_t DEFAULT_TRANSITION_COUNT,
          size_t DEFAULT_ACTION_COUNT>
StateMachine<Client_t>::StateMachine(
    Client_t* client, const TransitionArray<TRANSITION_COUNT>& transitions,
    const DefaultTransitionArray<DEFAULT_TRANSITION_COUNT>& default_transitions,
    const DefaultActionArray<DEFAULT_ACTION_COUNT>& default_actions)
    : client_{client} {}

}  // namespace aofsm

#endif  // AOFSM_SRC_STATE_MACHINE_H_
