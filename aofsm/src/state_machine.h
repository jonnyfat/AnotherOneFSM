// copyright Yevgen

#ifndef AOFSM_SRC_STATE_MACHINE_H_
#define AOFSM_SRC_STATE_MACHINE_H_

#include <cstddef>

namespace aofsm {

using std::size_t;

// Die Klasse implementiert eine State-Machine.
//
// Ein Client-Class kann es benutzen, um die eigenen Methoden
// Zustands- und Erreignis-abhängig aufzurufen.
//
// Dafür muss der Client-Class die Klasse StateMachine als Member instanziieren.
//
// Die Events und Zustände muss das Client-Class als Enums definieren.
//
// Die Transitionen von State-Machine als Array an Konstruktor von StateMachine
// übergeben.
//
// Beispiel:
//  - Das Client Class hat zwei Zustände: StateA , StateB und ein Ereignis
//    EventAction.
//  - Beim Ereignis EventAction muss
//    -  in StateA muss Methode DoA des Clients
//    -  in StateB muss Methode DoB des Clients
//
//  Das Client-Class muss die StateMachine auf folgende Weise benutzen:
//
//  - States und Events als nested Enum deklarieren
//      -  enum State { kStateA, kStateB, kStateCount, kInitState = kStateA};
//      -  enum Event { kEventAction , kEventCount };
//
//  - Transitionen werden als einen Array zusammengestellt.
//
//     {{kStateA, kEventAction, kStateA, &Client::DoA},
//      {kStateB, kEventAction, kStateB, &Client::DoB}}
//
//  - StateMachine wird als Member instantieert
//
//      An Konstruktor wird this-Pointer und Array mit Transitionen übergeben
//
//  - Der aktuelle Zustand kann SetCurrentState() gesetzt werden.
//
//  - Wenn das Client-Class die Methode Trigger(kEventAction) von
//    StateMachine-Istanz aufruft, wird zustandabhängig DoA oder DoB aufgerufen.
//
// Allgemeine Beschreibung der Schnittstellle:
//
//  Die State-Machine muss mit folgenden Parametern parametriert werden:
//  - Anzahl der Zustände
//  - Anzahl der Events
//  - Initialzustand
//  - Transitionen
//  - Default-Transitionen (optional)
//  - Default-Actions (optional)
//  - Zustand-Schachtelung (optional)
//
// Template-Parameter:
//  - Client_t - Client-Class
//  - State_t - Clients Enumeration für Zustände;
//  - Event_T - Clients Enumeration für Events
//
// Anforderungen Template-Parameter:
//
//  - Enum State_t muss Elemente kInitState und kStateCount haben:
//    -  enum State { .... , kStateCount, kInitState = ...};
//     -- kStateCount Anzahl der Zustände
//     -- kInitState - Initiale Zustand
//  - Enum Event_t muss Element kEventCount haben
//    -  enum Event { .... , kEventCount };
//     --  kEventCount - Anzahl der Events
//
//
template <typename Client_t, typename State_t = typename Client_t::State,
          typename Event_t = typename Client_t::Event,
          size_t MAX_ACTIONS_PER_TRANSITION = 2>
class StateMachine {
 public:
  // Pointer auf Member-Methode des Clients, welche beim Triggern von
  // State-Machine-Events aufgerufen werden.
  using Action_t = void (Client_t::*)(void);

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

  // Parametrierung eines Default-Übergangs für die State-Machine.
  // Es gilt für alle Zustädnde der Machine für welche keine Transition mit dem
  // Event definiert ist definiert ist.
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

template <typename Client_t, typename State_t, typename Event_t,
          size_t MAX_ACTIONS_PER_TRANSITION>
template <size_t TRANSITION_COUNT>
StateMachine<Client_t, State_t, Event_t, MAX_ACTIONS_PER_TRANSITION>::
    StateMachine(Client_t* client,
                 const TransitionArray<TRANSITION_COUNT>& transitions)
    : client_{client} {}

template <typename Client_t, typename State_t, typename Event_t,
          size_t MAX_ACTIONS_PER_TRANSITION>
template <size_t TRANSITION_COUNT, size_t DEFAULT_TRANSITION_COUNT>
StateMachine<Client_t, State_t, Event_t, MAX_ACTIONS_PER_TRANSITION>::
    StateMachine(Client_t* client,
                 const TransitionArray<TRANSITION_COUNT>& transitions,
                 const DefaultTransitionArray<DEFAULT_TRANSITION_COUNT>&
                     default_transitions)
    : client_{client} {}

template <typename Client_t, typename State_t, typename Event_t,
          size_t MAX_ACTIONS_PER_TRANSITION>
template <size_t TRANSITION_COUNT, size_t DEFAULT_ACTION_COUNT>
StateMachine<Client_t, State_t, Event_t, MAX_ACTIONS_PER_TRANSITION>::
    StateMachine(
        Client_t* client, const TransitionArray<TRANSITION_COUNT>& transitions,
        const DefaultActionArray<DEFAULT_ACTION_COUNT>& default_actions)
    : client_{client} {}

template <typename Client_t, typename State_t, typename Event_t,
          size_t MAX_ACTIONS_PER_TRANSITION>
template <size_t TRANSITION_COUNT, size_t DEFAULT_TRANSITION_COUNT,
          size_t DEFAULT_ACTION_COUNT>
StateMachine<Client_t, State_t, Event_t, MAX_ACTIONS_PER_TRANSITION>::
    StateMachine(
        Client_t* client, const TransitionArray<TRANSITION_COUNT>& transitions,
        const DefaultTransitionArray<DEFAULT_TRANSITION_COUNT>&
            default_transitions,
        const DefaultActionArray<DEFAULT_ACTION_COUNT>& default_actions)
    : client_{client} {}

template <typename Client_t, typename State_t, typename Event_t,
          size_t MAX_ACTIONS_PER_TRANSITION>
void StateMachine<Client_t, State_t, Event_t,
                  MAX_ACTIONS_PER_TRANSITION>::Trigger(Event_t event) {
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
