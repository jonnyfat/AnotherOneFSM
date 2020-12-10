// copyright Yevgen

#ifndef AOFSM_SRC_STATE_MACHINE_H_
#define AOFSM_SRC_STATE_MACHINE_H_

#include <cstddef>

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
//    -  enum State { .... , kStateCount, INITIAL_STATE = ...};
//    - kStateCount Anzahl der Zustände
//    - INITIAL_STATE - Initiale Zustand
//  - Enum Event_t muss Element kEventCount haben
//    -  enum Event { .... , kEventCount };
//    -  kEventCount - Anzahl der Events
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

  // Fasst mehrere Member-Methoden des Clients, welche bei einer Transition
  // aufgerufen werden.
  struct ArrayOfActions {
    template <class... Actions>
    ArrayOfActions(Actions... actions)
        : action_count{sizeof...(actions)}, action_array{actions...} {}

    size_t action_count{0};
    Action_t action_array[MAX_ACTIONS_PER_TRANSITION];
  };

  // Um Konfiguration von State-Machine zu vereinfachen werden alle Transitionen
  // als eine Datenstruktur von Typ Transition definiert.
  // Mit TransitionType, wird die Art der Transition vermerkt.
  enum class TransitionType {
    kDefaultAction,  ///< Default-Aktion für ein Event in beliebigen Zustand.
                     ///< Nur das Event und Actions sind angegeben.
                     ///< Gilt für alle Zustände in welchen für das Event keine
                     ///< DefaultTransitionen und keine Transition festgelegt
                     ///< sind.

    kDefaultTransition,  ///< Default-Transition für ein Event aus beliebigen
                         ///< Zustand fürhrt in einen anderen Zustand.
                         ///< Nur Event, Zielzustand und Actions sind angegeben.
                         ///< Gilt für alle Zustände in welchen für das Event
                         ///< keine Transition festgelegt ist.

    kTransition  ///< Transition für ein Event in einem Zustand führt in einen
                 ///< anderen Zustand.
                 ///< Gilt für alle Zustände für welche keine
                 ///< Transition und keine Transition festgelegt
                 ///< ist.
  };

  struct Transition {
   private:
    TransitionType transition_type_;
    State_t src_state_;
    Event_t event_;
    Guard_t guard_action_;
    State_t trans1_dst_state_;
    ArrayOfActions trans1_actions_;
    State_t trans2_dst_state_;
    ArrayOfActions trans2_actions_;

   public:
    // Parametrieren von Default-Action für ein Event der State-Machine.
    // Es gilt für alle Zustädnde der Machine für welche keine
    // Default-Transition und kein Transition für das Event parametriert ist.
    Transition(Event_t event, const ArrayOfActions& actions)
        : transition_type_{TransitionType::kDefaultAction},
          src_state_{State_t::kStateCount},         ///<  ungültig
          event_{event},                            ///< gültig
          guard_action_{nullptr},                   ///<  kein Guard
          trans1_dst_state_{State_t::kStateCount},  ///<  ungültig
          trans1_actions_{actions},                 ///< gültig
          trans2_dst_state_{State_t::kStateCount},  ///<  ungültig
          trans2_actions_{} {}                      ///<  leer

    // Parametrierung einer Default-Transition für ein Event der State-Machine.
    // Es gilt für alle Zustände der Machine für welche keine Transition für
    // das Event parametriert ist.
    Transition(Event_t event, State_t dst_state, const ArrayOfActions& actions)
        : transition_type_{TransitionType::kDefaultTransition},
          src_state_{State_t::kStateCount},         ///<  ungültig
          event_{event},                            ///< gültig
          guard_action_{nullptr},                   ///<  kein Guard
          trans1_dst_state_{dst_state},             ///< gültig
          trans1_actions_{actions},                 ///< gültig
          trans2_dst_state_{State_t::kStateCount},  ///<  ungültig
          trans2_actions_{} {}                      ///<  leer

    // Parametrierung einer unbedingten Transition für ein Event in einem
    // Zustand.
    Transition(State_t src_state, Event_t event, State_t dst_state,
               const ArrayOfActions& actions)
        : transition_type_{TransitionType::kTransition},
          src_state_{src_state},
          event_{event},
          guard_action_{nullptr},
          trans1_dst_state_{dst_state},
          trans1_actions_{actions},
          trans2_dst_state_{State_t::kStateCount},
          trans2_actions_{} {}

    // Parametrierung einer bedingten Transition für ein Event in einem
    // Zustand.
    Transition(State_t src_state, Event_t event, Guard_t guard_action,
               State_t dst_state_1, const ArrayOfActions& actions_1,
               State_t dst_state_2, const ArrayOfActions& actions_2)
        : transition_type_{TransitionType::kTransition},
          src_state_{src_state},
          event_{event},
          guard_action_{guard_action},
          trans1_dst_state_{dst_state_1},
          trans1_actions_{actions_1},
          trans2_dst_state_{dst_state_2},
          trans2_actions_{actions_2} {}
  };

  template <size_t N>
  using TransitionArray = Transition[N];

  template <size_t TRANSITION_COUNT>
  StateMachine(Client_t* client,
               const TransitionArray<TRANSITION_COUNT>& transitions);

  virtual ~StateMachine() = default;

  void Trigger(Event_t event, ActionParameterTypes... params);

  void SetCurrentState(State_t state);

 private:
  struct EventTransition {
    Guard_t guard_action;  ///< Bestimmt ob trans1 oder Trans2 ausgeführt wird
                           ///< trans1 : Transition, wenn guard_action nullptr
                           ///< oder liefert true
    State_t trans1_dst_state;
    ArrayOfActions trans1_actions;
    // trans1 : Transition, wenn guard_action nullptr oder liefert true
    State_t trans2_dst_state;
    ArrayOfActions trans2_actions;
  };
  struct StateTransitions {
    EventTransition event_transitions[Event_t::kEventCount];
  };

  Client_t* client_{nullptr};

  State_t current_state_{State_t::INITIAL_STATE};

  StateTransitions state_transitions_[State_t::kStateCount];
};

template <typename Client_t, typename State_t, typename Event_t,
          size_t MAX_ACTIONS_PER_TRANSITION, typename... ActionParameterTypes>
template <size_t TRANSITION_COUNT>
StateMachine<Client_t, State_t, Event_t, MAX_ACTIONS_PER_TRANSITION,
             ActionParameterTypes...>::
    StateMachine(Client_t* client,
                 const TransitionArray<TRANSITION_COUNT>& transitions)
    : client_{client} {}

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
    const ArrayOfActions* trans_actions = nullptr;

    Guard_t guard_action = current_transition.guard_action;

    if ((guard_action == nullptr) || ((client_->*guard_action)(params...))) {
      current_state_ = current_transition.trans1_dst_state;
      trans_actions = &current_transition.trans1_actions;
    } else {
      current_state_ = current_transition.trans2_dst_state;
      trans_actions = &current_transition.trans2_actions;
    }

    for (size_t i = 0; i < trans_actions->action_count; ++i) {
      Action_t action = trans_actions->action_array[i];
      (client_->*action)(params...);
    }
  }
}

}  // namespace aofsm

#endif  // AOFSM_SRC_STATE_MACHINE_H_
