// copyright Yevgen

#ifndef AOFSM_SRC_STATE_MACHINE_H_
#define AOFSM_SRC_STATE_MACHINE_H_

#include <cstddef>

#include "aofsm/src/array_of_actions.h"
#include "aofsm/src/transition.h"

namespace aofsm {

using std::size_t;

// Die Klasse StateMachine erlaubt es einem Client-Class die eigenen Methoden
// Zustands- und Erreignis-abhängig aufzurufen.
//
// Die Events und Zustände müssem als Enums definiert sein.
//
// Die Transition können als einen einfachen Array zusammengefasst werden und
// müssen an Konstruktor von StateMachine übergeben werden.
//
// Beispiel:
//
//  Statemachine
//  - zwei Zustände: StateA(Initial), StateB
//  - ein Ereignis: EventAction.
//  - zwei Transitionen:
//    -  StateA EventAction/DoA -> StateA
//    -  StateB EventAction/DoB -> StateB
//
//  Notwendige Konfiguration der StateMachine:
//
//  - States und Events als nested Enum in Client-Class:
//      -  enum State { kStateA, kStateB, kStateCount, INITIAL_STATE = kStateA};
//      -  enum Event { kEventAction , kEventCount };
//
//  - Transitionen werden als einen Array definiert.
//
//     {{kStateA, kEventAction, kStateA, &Client::DoA},
//      {kStateB, kEventAction, kStateB, &Client::DoB}}
//
//  Benutzung:
//
//  - StateMachine muss mit dem Client-Class als Template-Parameter instantiiert
//    werde,
//  - die StateMachine-Instanz kann ein Member von Client-Class sein
//
//  - an Konstruktor wird this-Pointer und Array mit Transitionen übergeben
//
//  - In diesem Beispiel kann der Zustand per SetCurrentState() gesetzt werden,
//    normaleweise über Transitionen von State-Machine.
//
//  - Wenn das Client-Class die Methode Trigger(kEventAction) von
//    StateMachine-Istanz aufruft, wird zustandabhängig DoA oder DoB von
//    Client-Class aufgerufen.
//
// Allgemeine Beschreibung der Schnittstellle:
//
//  Die State-Machine ist ein Variadic Template und muss mit folgenden
//  Parametern parametriert werden:
//  - Anzahl der Zustände ( kStateCount in enum State von Client-Class)
//  - Anzahl der Events (kEventCount in enum Event von Client-Class)
//  - Initialzustand (INITIAL_STATE in enum State von Client-Class)
//  - Transitionen (Array von StateMachine<>::Transition)
//
// Template-Parameter:
//  - Client_t - Client-Class
//  - State_t - Clients Enumeration für Zustände;
//  - Event_T - Clients Enumeration für Events
//  - MAX_ACTIONS_PER_TRANSITION - Maximale Anzahl von Actions pro Transition
//  - ActionParameterTypes - template parameter pack :  Signatur von
//    Action-Methoden. Wenn z.B. leer dann sind Action-Methoden void(void)
//
// Anforderungen Template-Parameter:
//
//  - Enum State_t muss Elemente INITIAL_STATE und kStateCount haben:
//    - enum State { .... , kStateCount, INITIAL_STATE = ...};
//    - kStateCount Anzahl der Zustände
//    - INITIAL_STATE - Initiale Zustand bei Instanziiierung
//    - Mehrere State-Konstanten, eine Konstante pro Zustand mit belibigen Namen
//
//    - Werte von State-Konstanten müssen
//      - im Beriech [0:kStateCount) liegen
//      - eindeutig sein
//
//  - Enum Event_t muss Element kEventCount haben
//    -  enum Event { .... , kEventCount };
//    -  kEventCount - Anzahl der Events
//
//    -  Werte von Event-Konstanten müssen
//      - im Beriech [0:kEventCount) liegen
//      - eindeutig sein
//
// Verwendung:
//
// Parametrierung: mit Client-Class und bei Bedarf mit anderen
//                 Template-Parameter parametrieren.
//
// Template-Parameter MAX_ACTIONS_PER_TRANSITION begrenzt maximale
//         Anzahl an Aktionen pro Transition.
//
// Instantiierung: mit ...
//             - Zeiger auf Client-Class-Instanz
//             - Tabelle mit Transitionen
//
// Tabelle mit Transitionen:
//   - Ganze Tabelle in {} - Klammern
//
//   - Jeder Eintrag in {} - Klammern
//
//   - Einträge mit ',' getrennt
//
// Eintragsarten in der Tabelle mit Transitionen:
//
//   - Default-Action für Event
//
//     * für ein Event in einem beleibiegen Zustand parametriert eine oder
//       mehrere Actionen
//
//     * kein Zustandswechsel
//
//     * gilt nur dann, wenn für das Event keine Default-Transition parametriert
//       ist
//
//     * gilt nur für Zustände, für welche keine Transition für das Event
//       parametriert ist.
//
//     * Format:
//      {Event, Action}
//      oder
//      {Event, {Action1,Action2, ....} }
//
//   - Default-Transition für Event
//
//     * für ein Event in einem beleibiegen Zustand parametriert eine oder
//       mehrere Actionen und Übergang in den Zielzustand
//
//     * überschreibt Default-Action für das Event, falls fälschlicherweise
//       parametriert ist.
//
//     * gilt nur für Zustände, für welche keine Transition für das Event
//       parametriert ist.
//
//     * Format:
//      {Event, Zielzustand, Action}
//      oder
//      {Event, Zielzustand, {Action1,Action2, ....} }
//
//   - Transition
//
//     * für ein Zustand und Event parametriert eine oder mehrere Actionen und
//       ein Übergang in einen Zielzustand
//
//     * überschreibt Default-Action oder Default-Transition für das Event in
//       dem Zustand
//
//     * Format:
//      {Zustand, Event, Zielzustand, Action}
//      oder
//      {Zustand, Event, Zielzustand, {Action1,Action2, ....} }
//
//   - Bedingte Transition
//
//     * für ein Zustand und Event parametriert einen Guard, welches bool
//       liefern bool, und zwei Transitionen(Zuelzustand,Actions) jeweils für
//       true und false Rückgabewerte von Guard.
//
//     * Funktionsweise: wenn im Zustand das Event getriggert wird, dann wird
//        guard aufgerufen.
//        Wenn Guard true liefert, wird transition 1 durchgeführt
//        Wenn Guard false liefert, wird transition 2 durchgeführt
//
//     * überschreibt Default-Action/Default-Transition für das Event in dem
//       Zustand
//
//
template <typename Client_t, typename State_t = typename Client_t::State,
          typename Event_t = typename Client_t::Event,
          size_t MAX_ACTIONS_PER_TRANSITION = 2,
          typename... ActionParameterTypes>
class StateMachine {
 public:
  // Pointer auf Member-Methode des Clients, welche bei einer Transition als
  // Action aufgerufen wird.
  using Action_t = void (Client_t::*)(ActionParameterTypes...);

  // Pointer auf Member-Methode des Clients, welche bei einer Guarded-Transition
  // als Guard dient.
  // Der Rückgabewert von Guard bestimmt, welche von zwei möglichen Transitionen
  // stattfinden soll.
  // Bei true wird 1. Transition ausgeführt, bei false die 2.
  using Guard_t = bool (Client_t::*)(ActionParameterTypes...) const;

  // Konstante für ungültier StateId
  static constexpr State_t kInvalidStateId = State_t::kStateCount;

  // Fasst Zeiger auf mehrere Member-Methoden des Clients, welche bei einer
  // Transition aufgerufen werden.
  using ArrayOfActions_t =
      internal::ArrayOfActions<MAX_ACTIONS_PER_TRANSITION, Action_t>;

  // Konfigurationselement für Transitionen
  // Wird benutzt um die Transitionen von State-Machine zu parametrieren.
  using Transition_t =
      Transition<State_t, Event_t, Guard_t, ArrayOfActions_t, kInvalidStateId>;

  // Um Konfiguration von State-Machine zu vereinfachen werden alle
  // Transitionen als eine Datenstruktur von Typ Transition definiert. Mit
  // TransitionType, wird die Art der Transition vermerkt.
  using TransitionType_t = typename Transition_t::TransitionType;

  struct StateInfo {
    const State_t state;

    const ArrayOfActions_t& on_entry_actions;

    const ArrayOfActions_t& on_exit_actions;

    const size_t sub_states_count{0};

    const State_t sub_states[State_t::kStateCount];

    template <class... SubStates>
    StateInfo(State_t state, const ArrayOfActions_t& on_entry_actions,
              const ArrayOfActions_t& on_exit_actions, SubStates... sub_states)
        : state{state},
          on_entry_actions{on_entry_actions},
          on_exit_actions{on_exit_actions},
          sub_states_count{sizeof...(sub_states)},
          sub_states{sub_states...} {}
  };

  template <size_t N>
  using TransitionArray = Transition_t[N];

  template <size_t N>
  using StateInfoArray = StateInfo[N];

  // Konstruktor: Initialisierung der State-Machine nur mit der
  // Transition-Konfiguration
  template <size_t TRANSITION_COUNT>
  StateMachine(Client_t* client,
               const TransitionArray<TRANSITION_COUNT>& transitions);

  // Konstruktor: Initialisierung der State-Machine mit der
  // Transition-Konfiguration und State-Konfiguration
  template <size_t TRANSITION_COUNT, size_t STATE_INFO_COUNT>
  StateMachine(Client_t* client,
               const TransitionArray<TRANSITION_COUNT>& transitions,
               const StateInfoArray<STATE_INFO_COUNT>& state_infos);

  virtual ~StateMachine() = default;

  void Trigger(Event_t event, ActionParameterTypes... params);

  void SetCurrentState(State_t state);

 private:
  void SetupTransitions(const Transition_t* transitions,
                        size_t transition_count);

  void SetupDefaultAction(const Transition_t& transition);

  void SetupDefaultTransition(const Transition_t& transition);

  void SetupTransition(const Transition_t& transition);

  void SetupConditionalTransition(const Transition_t& transition);

  void SetupStates(const StateInfo* state_infos, size_t state_info_count);

  void SetupSubStates(State_t state, const State_t* substates,
                      size_t substates_count);

  struct EventTransition {
    Guard_t guard_action;  ///< Bestimmt ob trans1 oder Trans2 ausgeführt wird
                           ///< trans1 : Transition, wenn guard_action nullptr
                           ///< oder liefert true

    // trans1 : Transition, wenn guard_action nullptr oder liefert true
    State_t trans1_dst_state;
    ArrayOfActions_t trans1_actions;

    // trans2 : Transition, wenn guard_action liefert false
    State_t trans2_dst_state;
    ArrayOfActions_t trans2_actions;

    void SetupDefaultAction(const ArrayOfActions_t& actions) {
      guard_action = nullptr;
      trans1_dst_state = kInvalidStateId;
      trans1_actions = actions;
      trans2_dst_state = kInvalidStateId;
      trans2_actions.SetEmpty();
    }
  };
  struct StateTransitions {
    bool has_substates_{false};

    State_t parent_state{kInvalidStateId};

    ArrayOfActions_t on_entry_actions;

    ArrayOfActions_t on_exit_actions;

    EventTransition event_transitions[Event_t::kEventCount];
  };

  Client_t* client_{nullptr};

  State_t current_state_{State_t::INITIAL_STATE};

  StateTransitions state_transitions_[State_t::kStateCount];
};

template <typename Client_t, typename State_t, typename Event_t,
          size_t MAX_ACTIONS_PER_TRANSITION, typename... ActionParameterTypes>
void StateMachine<
    Client_t, State_t, Event_t, MAX_ACTIONS_PER_TRANSITION,
    ActionParameterTypes...>::Trigger(Event_t event,
                                      ActionParameterTypes... params) {
  if (current_state_ < State_t::kStateCount && event < Event_t::kEventCount) {
    // Transaction in aktuellem Zustand für den aktuellen Event.
    const EventTransition& current_transition =
        state_transitions_[current_state_].event_transitions[event];

    // Die Actions, welche beim Zustandsübergang ausgeführt werden müssen sind
    // von Guard abhängig
    const ArrayOfActions_t* transition_action = nullptr;

    Guard_t guard_action = current_transition.guard_action;

    if ((guard_action == nullptr) || ((client_->*guard_action)(params...))) {
      current_state_ = current_transition.trans1_dst_state;
      transition_action = &current_transition.trans1_actions;
    } else {
      current_state_ = current_transition.trans2_dst_state;
      transition_action = &current_transition.trans2_actions;
    }

    transition_action->CallFor(client_, params...);
  }
}

template <typename Client_t, typename State_t, typename Event_t,
          size_t MAX_ACTIONS_PER_TRANSITION, typename... ActionParameterTypes>
void StateMachine<Client_t, State_t, Event_t, MAX_ACTIONS_PER_TRANSITION,
                  ActionParameterTypes...>::SetCurrentState(State_t state) {
  current_state_ = state;
}

template <typename Client_t, typename State_t, typename Event_t,
          size_t MAX_ACTIONS_PER_TRANSITION, typename... ActionParameterTypes>
template <size_t TRANSITION_COUNT>
StateMachine<Client_t, State_t, Event_t, MAX_ACTIONS_PER_TRANSITION,
             ActionParameterTypes...>::
    StateMachine(Client_t* client,
                 const TransitionArray<TRANSITION_COUNT>& transitions)
    : client_{client} {
  SetupTransitions(transitions, TRANSITION_COUNT);
}

template <typename Client_t, typename State_t, typename Event_t,
          size_t MAX_ACTIONS_PER_TRANSITION, typename... ActionParameterTypes>
template <size_t TRANSITION_COUNT, size_t STATE_INFO_COUNT>
StateMachine<Client_t, State_t, Event_t, MAX_ACTIONS_PER_TRANSITION,
             ActionParameterTypes...>::
    StateMachine(Client_t* client,
                 const TransitionArray<TRANSITION_COUNT>& transitions,
                 const StateInfoArray<STATE_INFO_COUNT>& state_infos)
    : client_{client} {
  SetupTransitions(transitions, TRANSITION_COUNT);
  SetupStates(state_infos, STATE_INFO_COUNT);
}

template <typename Client_t, typename State_t, typename Event_t,
          size_t MAX_ACTIONS_PER_TRANSITION, typename... ActionParameterTypes>
void StateMachine<
    Client_t, State_t, Event_t, MAX_ACTIONS_PER_TRANSITION,
    ActionParameterTypes...>::SetupTransitions(const Transition_t* transitions,
                                               size_t transition_count) {
  auto end = transitions + transition_count;
  for (auto transition = transitions; transition != end; ++transition) {
    switch (transition->transition_type) {
      case TransitionType_t::kDefaultAction:
        SetupDefaultAction(*transition);
        break;
      case TransitionType_t::kDefaultTransition:
        SetupDefaultTransition(*transition);
        break;
      case TransitionType_t::kTransition:
        SetupTransition(*transition);
        break;
      case TransitionType_t::kConditionalTransition:
        SetupConditionalTransition(*transition);
        break;
    }
  }
}

template <typename Client_t, typename State_t, typename Event_t,
          size_t MAX_ACTIONS_PER_TRANSITION, typename... ActionParameterTypes>
void StateMachine<
    Client_t, State_t, Event_t, MAX_ACTIONS_PER_TRANSITION,
    ActionParameterTypes...>::SetupStates(const StateInfo* state_infos,
                                          size_t state_info_count) {
  auto end = state_infos + state_info_count;
  for (auto state_info = state_infos; state_info != end; ++state_info) {
    auto& state = state_transitions_[state_info->state];
    state.on_entry_actions = state_info->on_entry_actions;
    state.on_exit_actions = state_info->on_exit_actions;
    SetupSubStates(state_info->state, state_info->sub_states,
                   state_info->sub_states_count);
  }
}

template <typename Client_t, typename State_t, typename Event_t,
          size_t MAX_ACTIONS_PER_TRANSITION, typename... ActionParameterTypes>
void StateMachine<Client_t, State_t, Event_t, MAX_ACTIONS_PER_TRANSITION,
                  ActionParameterTypes...>::
    SetupDefaultAction(const Transition_t& transition) {
  auto event = transition.event;

  for (auto state : state_transitions_) {
    state.event_transitions[event].SetupDefaultAction(transition.trans1_action);
  }
}

template <typename Client_t, typename State_t, typename Event_t,
          size_t MAX_ACTIONS_PER_TRANSITION, typename... ActionParameterTypes>
void StateMachine<Client_t, State_t, Event_t, MAX_ACTIONS_PER_TRANSITION,
                  ActionParameterTypes...>::
    SetupDefaultTransition(const Transition_t& transition) {}

template <typename Client_t, typename State_t, typename Event_t,
          size_t MAX_ACTIONS_PER_TRANSITION, typename... ActionParameterTypes>
void StateMachine<Client_t, State_t, Event_t, MAX_ACTIONS_PER_TRANSITION,
                  ActionParameterTypes...>::SetupTransition(const Transition_t&
                                                                transition) {}

template <typename Client_t, typename State_t, typename Event_t,
          size_t MAX_ACTIONS_PER_TRANSITION, typename... ActionParameterTypes>
void StateMachine<Client_t, State_t, Event_t, MAX_ACTIONS_PER_TRANSITION,
                  ActionParameterTypes...>::
    SetupConditionalTransition(const Transition_t& transition) {}

template <typename Client_t, typename State_t, typename Event_t,
          size_t MAX_ACTIONS_PER_TRANSITION, typename... ActionParameterTypes>
void StateMachine<
    Client_t, State_t, Event_t, MAX_ACTIONS_PER_TRANSITION,
    ActionParameterTypes...>::SetupSubStates(State_t state,
                                             const State_t* substates,
                                             size_t substates_count) {}

}  // namespace aofsm

#endif  // AOFSM_SRC_STATE_MACHINE_H_
