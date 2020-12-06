// copyright Yevgen

#ifndef AOFSM_SRC_STATE_MACHINE_H_
#define AOFSM_SRC_STATE_MACHINE_H_

#include <cstddef>

namespace aofsm {

using std::size_t;

// Die Klasse implementiert eine State-Machine für eine Client-Classe.
//
// Es erlaubt der Client-Classe die Methoden der Klasse in Abhängikeit vom einem
// Zustand aufzurufen.
//
// Z.B. nehmen wir an die Client-Classe kann zwei Zustände haben
//  - StateA und StateB.
//
// In einer Situation muss im Zustand StateA die Client-Methode DoA und
// in StateB die Client-Methode StateB aufgerufen werden.
//
// In diesem Fall kann der Client die StateMachine auf folgende Weise benutzen:
// - Client muss als nested Dekalration
//      - ein enum State haben:
//         enum State { kStateA, kStateB, kStateCount, kInitState = kStateA};
//      - ein enum Event haben:
//         enum Event { kEventAction , kEventCount };
//  - Client muss eine Instanz von StateMachine state_machint instanziieren und
//       mit folgenden Transitionen parametrieren.
//
//     {{kStateA, kEventAction, kStateA, &Client::DoA},
//      {kStateB, kEventAction, kStateB, &Client::DoB}}
//
//  - Den Zustand mit SetCurrentState() setzen.
//
//  - Wenn der Cleint Trigger(kEventAction) wird zustandabhängig DoA oder DoB
//       aufgerufen.
//
//
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
template <typename Client_t, size_t MAX_ACTIONS_PER_TRANSITION = 2>
class StateMachine {
 public:
  // Pointer auf Member-Methode des Clients, welche beim Triggern von
  // State-Machine-Events aufgerufen werden.
  using Action_t = void (Client_t::*)(void);
  // Clients Enumeration für Zustände
  using State_t = typename Client_t::State;
  // Clients Enumeration für Events
  using Event_t = typename Client_t::Event;

  // Array von Pointern auf Client-Member-Methoden.
  struct ArrayOfActions {
    template <class... Actions>
    ArrayOfActions(Actions... actions)
        : action_count{sizeof...(actions)}, action_array{actions...} {}

    size_t action_count{0};
    Action_t action_array[MAX_ACTIONS_PER_TRANSITION];
  };

  // Parametrierung eines Übergangs für ein Zustand:
  // im Zustand src_state beim Event event soll
  // Übergang nach Zustand dst_state stattfinden und dabei die die
  // Client-Methoden aus ArrayOfActions actions aufgerufen werden.
  struct StateTransitionDef {
    State_t src_state;
    Event_t event;
    State_t dst_state;
    ArrayOfActions actions;
  };

  // Parametrierung eines default Übergangs für die State-Machinte:
  struct StateMachineDefaultTransitionDef {
    Event_t event;
    State_t dst_state;
    ArrayOfActions actions;
  };

  struct DefaultAction {
    Event_t event;
    ArrayOfActions actions;
  };

  template <size_t N>
  using TransitionArray = StateTransitionDef[N];

  template <size_t N>
  using DefaultTransitionArray = StateMachineDefaultTransitionDef[N];

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

  void Trigger(Event_t event);

  void SetCurrentState(State_t state);

 private:
  struct EventTransition {
    State_t dst_state;
    ArrayOfActions actions;
  };
  struct StateTransitions {
    EventTransition event_transitions[Event_t::kEventCount];
  };

  Client_t* client_{nullptr};

  State_t current_state_{State_t::kInitState};

  StateTransitions state_transitions_[State_t::kStateCount];
};

template <typename Client_t, size_t MAX_ACTIONS_PER_TRANSITION>
template <size_t TRANSITION_COUNT>
StateMachine<Client_t, MAX_ACTIONS_PER_TRANSITION>::StateMachine(
    Client_t* client, const TransitionArray<TRANSITION_COUNT>& transitions)
    : client_{client} {}

template <typename Client_t, size_t MAX_ACTIONS_PER_TRANSITION>
template <size_t TRANSITION_COUNT, size_t DEFAULT_TRANSITION_COUNT>
StateMachine<Client_t, MAX_ACTIONS_PER_TRANSITION>::StateMachine(
    Client_t* client, const TransitionArray<TRANSITION_COUNT>& transitions,
    const DefaultTransitionArray<DEFAULT_TRANSITION_COUNT>& default_transitions)
    : client_{client} {}

template <typename Client_t, size_t MAX_ACTIONS_PER_TRANSITION>
template <size_t TRANSITION_COUNT, size_t DEFAULT_ACTION_COUNT>
StateMachine<Client_t, MAX_ACTIONS_PER_TRANSITION>::StateMachine(
    Client_t* client, const TransitionArray<TRANSITION_COUNT>& transitions,
    const DefaultActionArray<DEFAULT_ACTION_COUNT>& default_actions)
    : client_{client} {}

template <typename Client_t, size_t MAX_ACTIONS_PER_TRANSITION>
template <size_t TRANSITION_COUNT, size_t DEFAULT_TRANSITION_COUNT,
          size_t DEFAULT_ACTION_COUNT>
StateMachine<Client_t, MAX_ACTIONS_PER_TRANSITION>::StateMachine(
    Client_t* client, const TransitionArray<TRANSITION_COUNT>& transitions,
    const DefaultTransitionArray<DEFAULT_TRANSITION_COUNT>& default_transitions,
    const DefaultActionArray<DEFAULT_ACTION_COUNT>& default_actions)
    : client_{client} {}

template <typename Client_t, size_t MAX_ACTIONS_PER_TRANSITION>
void StateMachine<Client_t, MAX_ACTIONS_PER_TRANSITION>::Trigger(
    Event_t event) {
  if (current_state_ < State_t::kStateCount && event < Event_t::kEventCount) {
    const EventTransition& current_transition =
        state_transitions_[current_state_].event_transitions[event];

    current_state_ = current_transition.dst_state;
    for (size_t i = 0; i < current_transition.actions.action_count; ++i) {
      Action_t action = current_transition.actions.action_array[i];
      (client_->*action)();
    }
  }
}

}  // namespace aofsm

#endif  // AOFSM_SRC_STATE_MACHINE_H_
