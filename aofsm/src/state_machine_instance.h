// copyright Yevgen

#ifndef AOFSM_SRC_STATE_MACHINE_INSTANCE_H_
#define AOFSM_SRC_STATE_MACHINE_INSTANCE_H_

#include "aofsm/src/std_types.h"

#include "aofsm/src/internal/state_machine_context.h"
#include "aofsm/src/internal/state_machine_transition.h"
#include "aofsm/src/internal/state_machine_transition_action.h"
#include "aofsm/src/internal/state_machine_transition_table.h"

namespace aofsm {

// Die Klasse StateMachineInstance erlaubt es einem Client-Class die eigenen
// Methoden Zustands- und Erreignis-abhängig aufzurufen.
//
// Die Events und Zustände müssem als Enums definiert sein.
//
// Die Transition müssen als eine StateMachineDescription-Instanz an Konstruktor
// von StateMachineInstance übergeben werden.
//
//  Benutzung:
//
//  - Definieren von Client-StateMachineInstance-Alias aus
//  StateMachineInstance-Template.
//    Der StateMachineInstance-Template muss im einfachsten Fall nur mit
//    Client-Class als Template-Parameter definiert werden, wobei bei
//    Client-Class folgende Voraussetzungen erfüllt werden müssen:
//
//     - Der Client-Class muss zwei nested enum-Deklatarionen haben:
//
//       - enum State mit Elementen INITIAL_STATE, kStateCount und
//          State-Konstanten, welche für Parametrierung von
//          StateMachineDescription verwendet werden.
//           Beispiel: enum State { .... , kStateCount, INITIAL_STATE = ...};
//           kStateCount - Anzahl der Zustände
//           INITIAL_STATE - Initiale Zustand bei Instanziiierung
//           State-Konstanten, eine Konstante pro Zustand mit belibigen Namen
//         Werte von State-Konstanten müssen im Bereich [0:kStateCount) liegen
//           und eindeutig sein
//
//       - enum Event mit Element kEventCount haben
//          Beispiel: enum Event { .... , kEventCount };
//           kEventCount - Anzahl der Events
//         Werte von Event-Konstanten müssen im Beriech [0:kEventCount) liegen
//           und eindeutig sein
//
//  - StateMachineDescription muss als static const Member von Client-Class
//    instantiiert werden.
//    Instanz-Typ:
//    Client-StateMachineInstance-Alias::StateMachineTransitionTable_t.
//
//  - StateMachineInstance muss als Member von Client-Class instantiiert werden,
//    Instanz-Typ: Client-StateMachineInstance-Alias.
//    - Konstruktorparamer:
//        - Pointer auf Client-Class
//        - Referenz auf StateMachineDescription.
//
//  - Mit der Methode Trigger() können die Zustandsübergänge der State-Machine
//    getriggert werden. Sie werden entsprechend der Konfiguration von
//    StateMachineDescription ausgeführt.
//
// Allgemeine Beschreibung der Schnittstellle:
//
//  Die State-Machine-Implementierung benötigt von Client folgende
//  Informationen:
//  -
//  - Anzahl der Events (kEventCount in enum Event von Client-Class)
//  - Initialzustand (INITIAL_STATE in enum State von Client-Class)
//  - Transitionen (StateMachineDescription)
//
// Template-Parameter:
//  - Client - Client-Class
//  - State - Enumeration für Zustände.
//      - State  muss Anzahl der Zustände als enum-Element kStateCount
//        enthalten.
//      - State  muss Id von Initalzustand als enum-Element INITIAL_STATE
//        enthalten.
//  - Event - Enumeration für Events.
//      - Event muss Anzahl der Events als enum-Element kEventCount
//        enthalten.
//  - MAX_ACTIONS_PER_TRANSITION - Maximale Anzahl von Actions pro Transition
//  - ActionParameterTypes - template parameter pack :  Signatur von
//    Action-Methoden. Wenn leer dann sind Action-Methoden void(void)
//
template <typename Client, size_t MAX_ACTIONS_PER_TRANSITION = 1,
          typename... ActionParameterTypes>
class StateMachineInstance {
 public:
  using State = typename Client::State;
  using Event = typename Client::Event;

  // Pointer auf Member-Methode des Clients, welche bei einer Transition als
  // Action aufgerufen wird.
  using ActionStep_t = void (Client::*)(ActionParameterTypes...);

  // Pointer auf Member-Methode des Clients, welche bei einer Guarded-Transition
  // als Guard dient.
  // Der Rückgabewert von Guard bestimmt, welche von zwei möglichen Transitionen
  // stattfinden soll.
  // Bei true wird 1. Transition ausgeführt, bei false die 2.
  using Guard_t = bool (Client::*)(ActionParameterTypes...) const;

  // Fasst Zeiger auf mehrere Member-Methoden des Clients, welche bei einer
  // Transition aufgerufen werden.
  using ArrayOfActions_t =
      internal::StateMachineTransitionAction<MAX_ACTIONS_PER_TRANSITION,
                                             ActionStep_t>;

  using Context_t =
      StateMachineContext<Client, State, Event, ArrayOfActions_t, Guard_t,
                          State::kStateCount, Event::kEventCount>;

  using StateMachineTransitionTable_t = StateMachineTransitionTable<Context_t>;

  using EventTransition_t =
      typename StateMachineTransitionTable_t::TransitionForStateAndEvent_t;

  // Konstruktor: Initialisierung der State-Machine nur mit der
  // Transition-Konfiguration
  StateMachineInstance(Client* client,
                       const StateMachineTransitionTable_t& transition_table_);

  virtual ~StateMachineInstance() = default;

  void Trigger(Event event, ActionParameterTypes... params);

  void SetCurrentState(State state) { current_state_ = state; }

  State GetCurrentState() const { return current_state_; }

 private:
  Client* client_{nullptr};

  const StateMachineTransitionTable_t& transition_table__;

  State current_state_{State::INITIAL_STATE};
};

template <typename Client, size_t MAX_ACTIONS_PER_TRANSITION,
          typename... ActionParameterTypes>
StateMachineInstance<Client, MAX_ACTIONS_PER_TRANSITION,
                     ActionParameterTypes...>::
    StateMachineInstance(Client* client,
                         const StateMachineTransitionTable_t& transition_table_)
    : client_{client}, transition_table__{transition_table_} {}

template <typename Client, size_t MAX_ACTIONS_PER_TRANSITION,
          typename... ActionParameterTypes>
void StateMachineInstance<
    Client, MAX_ACTIONS_PER_TRANSITION,
    ActionParameterTypes...>::Trigger(Event event,
                                      ActionParameterTypes... params) {
  if (current_state_ < State::kStateCount && event < Event::kEventCount) {
    // Transaction in aktuellem Zustand für den aktuellen Event.
    const EventTransition_t& current_transition =
        transition_table__.GetTransition(current_state_, event);

    // Die Actions, welche beim Zustandsübergang ausgeführt werden müssen sind
    // von Guard abhängig
    const ArrayOfActions_t* transition_action = nullptr;

    Guard_t guard_action = current_transition.guard_action;

    if ((guard_action == nullptr) || ((client_->*guard_action)(params...))) {
      current_state_ = current_transition.trans1_dst_state;
      transition_action = &current_transition.trans1_action;
    } else {
      current_state_ = current_transition.trans2_dst_state;
      transition_action = &current_transition.trans2_action;
    }

    transition_action->CallFor(client_, params...);
  }
}

}  // namespace aofsm

#endif  // AOFSM_SRC_STATE_MACHINE_INSTANCE_H_
