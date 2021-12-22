// copyright Yevgen

#ifndef AOFSM_SRC_STATE_MACHINE_V1_STATE_MACHINE_H_
#define AOFSM_SRC_STATE_MACHINE_V1_STATE_MACHINE_H_

#include "aofsm/src/std_types.h"

#include "aofsm/src/internal/array_of_actions.h"
#include "aofsm/src/internal/state_machine_context.h"
#include "aofsm/src/internal/state_machine_description.h"
#include "aofsm/src/internal/transition.h"

// Macro für Deklaration von State-Machine im Rumpf der Klasse
// class SimlpeClient1 {
//  ...;
//  DECL_STATE_MACHINE(SimlpeClient1, state_machine_);
// };

#define DECL_STATE_MACHINE(__class_name__, __member_name__)                    \
  friend class aofsm::StateMachine<__class_name__>;                            \
  using StateMachine_t = aofsm::StateMachine<__class_name__>;                  \
  using StateMachineDescription_t = StateMachine_t::StateMachineDescription_t; \
  static const StateMachineDescription_t __member_name__##description;         \
  StateMachine_t __member_name__{this, __member_name__##description};

#define DECL_STATE_MACHINE_WITH_MULT_ACTIONS(__class_name__, __member_name__,  \
                                             __action_count__)                 \
  friend class aofsm::StateMachine<__class_name__, __action_count__>;          \
  using StateMachine_t =                                                       \
      aofsm::StateMachine<__class_name__, __action_count__>;                   \
  using StateMachineDescription_t = StateMachine_t::StateMachineDescription_t; \
  static const StateMachineDescription_t __member_name__##description;         \
  StateMachine_t __member_name__{this, __member_name__##description};

#define DECL_STATE_MACHINE_WITH_MULT_ACTIONS_AND_TYPES(                        \
    __class_name__, __member_name__, __action_count__, ...)                    \
  friend class aofsm::StateMachine<__class_name__, __action_count__,           \
                                   __VA_ARGS__>;                               \
  using StateMachine_t =                                                       \
      aofsm::StateMachine<__class_name__, __action_count__, __VA_ARGS__>;      \
  using StateMachineDescription_t = StateMachine_t::StateMachineDescription_t; \
  static const StateMachineDescription_t __member_name__##description;         \
  StateMachine_t __member_name__{this, __member_name__##description};

// Macro für Definition von State-Machine im cpp-Datei der Klasse
// Achtung: 2x"{"-Klammer am Anfang und 2x"}"-Klammer am Ende!
// DEF_STATE_MACHINE(SimlpeClient1, state_machine_){
//    {{INITIAL_STATE, kStartAEvt, A_STATE, &DoStartA},
//     {INITIAL_STATE, kStartBEvt, B_STATE, &DoStartB},
//     {A_STATE, kEndEvt, FINAL_STATE, &DoEndA},
//     {B_STATE, kEndEvt, FINAL_STATE, &DoEndB}}};
#define DEF_STATE_MACHINE(__class_name__, __member_name__) \
  const __class_name__::StateMachineDescription_t          \
      __class_name__::__member_name__##description

namespace aofsm {

// Die Klasse StateMachine erlaubt es einem Client-Class die eigenen Methoden
// Zustands- und Erreignis-abhängig aufzurufen.
//
// Die Events und Zustände müssem als Enums definiert sein.
//
// Die Transition müssen als eine StateMachineDescription-Instanz an Konstruktor
// von StateMachine übergeben werden.
//
//  Benutzung:
//
//  - Definieren von Client-StateMachine-Alias aus StateMachine-Template.
//    Der StateMachine-Template muss im einfachsten Fall nur mit Client-Class
//    als Template-Parameter definiert werden, wobei bei Client-Class
//    folgende Voraussetzungen erfüllt werden müssen:
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
//    Instanz-Typ: Client-StateMachine-Alias::StateMachineDescription_t.
//
//  - StateMachine muss als Member von Client-Class instantiiert werden,
//    Instanz-Typ: Client-StateMachine-Alias.
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
class StateMachine {
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
      internal::ArrayOfActions<MAX_ACTIONS_PER_TRANSITION, ActionStep_t>;

  using Context_t =
      StateMachineContext<Client, State, Event, ArrayOfActions_t, Guard_t,
                          State::kStateCount, Event::kEventCount>;

  using StateMachineDescription_t = StateMachineDescription<Context_t>;

  using EventTransition_t =
      typename StateMachineDescription_t::TransitionForStateAndEvent_t;

  // Konstruktor: Initialisierung der State-Machine nur mit der
  // Transition-Konfiguration
  StateMachine(Client* client, const StateMachineDescription_t& description);

  virtual ~StateMachine() = default;

  void Trigger(Event event, ActionParameterTypes... params);

  void SetCurrentState(State state) { current_state_ = state; }

  State GetCurrentState() const { return current_state_; }

 private:
  Client* client_{nullptr};

  const StateMachineDescription_t& description_;

  State current_state_{State::INITIAL_STATE};
};

template <typename Client, size_t MAX_ACTIONS_PER_TRANSITION,
          typename... ActionParameterTypes>
StateMachine<Client, MAX_ACTIONS_PER_TRANSITION, ActionParameterTypes...>::
    StateMachine(Client* client, const StateMachineDescription_t& description)
    : client_{client}, description_{description} {}

template <typename Client, size_t MAX_ACTIONS_PER_TRANSITION,
          typename... ActionParameterTypes>
void StateMachine<Client, MAX_ACTIONS_PER_TRANSITION, ActionParameterTypes...>::
    Trigger(Event event, ActionParameterTypes... params) {
  if (current_state_ < State::kStateCount && event < Event::kEventCount) {
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

}  // namespace aofsm

#endif  // AOFSM_SRC_STATE_MACHINE_V1_STATE_MACHINE_H_
