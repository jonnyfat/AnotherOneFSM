// copyright Yevgen

#ifndef AOFSM_SRC_STATE_MACHINE_H_
#define AOFSM_SRC_STATE_MACHINE_H_

#include <cstddef>

#include "aofsm/src/array_of_actions.h"
#include "aofsm/src/state_machine_description.h"
#include "aofsm/src/transition.h"

namespace aofsm {

using std::size_t;

// Die Klasse StateMachine erlaubt es einem Client-Class die eigenen Methoden
// Zustands- und Erreignis-abhängig aufzurufen.
//
// Die Events und Zustände müssem als Enums definiert sein.
//
// Die Transition müssem als eine StateMachineDescription-Instanz an Konstruktor
// von StateMachine übergeben werden.
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
//
//  - StateMachine-Class muss aus StateMachine-Template mit Client-Class und
//    weiteren parameter instantiiert werden.
//
//  - StateMachineDescription ( StateMachine-Class::StateMachineDescription_t )
//    muss instantiiert werden (normaleweise als static const)
//
//  - an Konstruktor von StateMachineDescription muss Array mit Transitionen
//    übergeben
//
//  - StateMachine muss instantiiert werden (ein Member von Client-Class)
//
//  - an Konstruktor von StateMachine muss die Instanz von Client-Class und von
//    StateMachineDescription übergeben werden.
//
//  - Wenn das Client-Class die Methode Trigger(kEventAction) von
//    StateMachine-Istanz aufruft, wird zustandabhängig DoA oder DoB von
//    Client-Class aufgerufen.
//
// Allgemeine Beschreibung der Schnittstellle:
//
//  Die State-Machine-Implementierung benötigt von Client folgende
//  Informationen:
//  - Anzahl der Zustände ( kStateCount in enum State von Client-Class)
//  - Anzahl der Events (kEventCount in enum Event von Client-Class)
//  - Initialzustand (INITIAL_STATE in enum State von Client-Class)
//  - Transitionen (StateMachineDescription)
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
//             - StateMachineDescription
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

  // Fasst Zeiger auf mehrere Member-Methoden des Clients, welche bei einer
  // Transition aufgerufen werden.
  using ArrayOfActions_t =
      internal::ArrayOfActions<MAX_ACTIONS_PER_TRANSITION, Action_t>;

  using StateMachineDescription_t =
      StateMachineDescription<State_t, Event_t, ArrayOfActions_t, Guard_t>;

  using EventTransition_t = typename StateMachineDescription_t::EventTransition;

  // Konstruktor: Initialisierung der State-Machine nur mit der
  // Transition-Konfiguration
  StateMachine(Client_t* client, const StateMachineDescription_t& description);

  virtual ~StateMachine() = default;

  void Trigger(Event_t event, ActionParameterTypes... params);

  void SetCurrentState(State_t state);

 private:
  Client_t* client_{nullptr};

  const StateMachineDescription_t& description_;

  State_t current_state_{State_t::INITIAL_STATE};
};

template <typename Client_t, typename State_t, typename Event_t,
          size_t MAX_ACTIONS_PER_TRANSITION, typename... ActionParameterTypes>
StateMachine<Client_t, State_t, Event_t, MAX_ACTIONS_PER_TRANSITION,
             ActionParameterTypes...>::
    StateMachine(Client_t* client, const StateMachineDescription_t& description)
    : client_{client}, description_{description} {}

template <typename Client_t, typename State_t, typename Event_t,
          size_t MAX_ACTIONS_PER_TRANSITION, typename... ActionParameterTypes>
void StateMachine<
    Client_t, State_t, Event_t, MAX_ACTIONS_PER_TRANSITION,
    ActionParameterTypes...>::Trigger(Event_t event,
                                      ActionParameterTypes... params) {
  if (current_state_ < State_t::kStateCount && event < Event_t::kEventCount) {
    // Transaction in aktuellem Zustand für den aktuellen Event.
    const EventTransition_t& current_transition =
        description_.GetTransition(current_state_, event);

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

}  // namespace aofsm

#endif  // AOFSM_SRC_STATE_MACHINE_H_
