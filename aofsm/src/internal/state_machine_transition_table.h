// copyright Yevgen

#ifndef AOFSM_SRC_INTERNAL_STATE_MACHINE_TRANSITION_TABLE_H_
#define AOFSM_SRC_INTERNAL_STATE_MACHINE_TRANSITION_TABLE_H_

#include "aofsm/src/std_types.h"

#include "aofsm/src/internal/state_machine_transition.h"
#include "aofsm/src/internal/state_machine_transition_data.h"

namespace aofsm {

// StateMachineTransitionTable erzeugen es aus einer Menge von
// erlaubten Statemachine-Transitionen {SrcState,  Event, DstState, Action}
// einen Map:
//
//    {SrcState,Event} => {DstState,Action}
//
// Die Zustände und Events  müssen als Enums State_t und Event_t definiert sein.
//
// Die Statemachine-Transitionen müssen als Transition-Array  als
// Kosntruktor-Parameter übergeben werden.
//
// Beispiel:
//
//  Für folgende Statemachine
//  - zwei States: StateA, StateB
//  - ein Event: EventX
//  - zwei Actions: DoA DoB
//  - zwei Transitionen:
//    -  StateA -> EventX/DoA -> StateB
//    -  StateB -> EventX/DoB -> StateA
//
//  muss die Client-Classe
//
//  - zwei nested Enums haben: :
//      -  enum State { kStateA, kStateB, kStateCount};
//      -  enum Event { kEventX , kEventCount };
//
//  - Transition-Array :
//
//     {kStateA, kEventX, kStateB, DoA},
//     {kStateB, kEventX, kStateA, DoB}
//
// Allgemeine Beschreibung der Schnittstellle:
//
//  Die StateMachineTransitionTable ist ein Template mit Parametern Context:
//  Der Context muss folgendes haben:
//  - State_t : Datentyp für State-Id, muss enum sein und folgende Konstante
//              kStateCount haben
//              kStateCount - Anzahl der Zustände
//  - Event_t : Datentyp für Event, muss enum sein und folgende Konstante
//              kEventCount haben
//              kEventCount - Anzahl der Events
//  - Action_t, Guard_t : Datentypen welche für jede Transition {SrcState,Event}
//                        gespeichert werden.  Müssen kopierbar sein.
//
// Parametrierung:
//
// Bei Instanziierung muss ein TransitionArray als Konstruktor-Parameter
// übergeben werden. Es ist nur für und während der Instantiierung notwendig,
// danach wird es nicht mehr benötigt.
//
// TransitionArray:
//   - Ganz in {} - Klammern
//
//   - Jeder Eintrag in {} - Klammern
//
//   - Einträge mit ',' getrennt
//
// Eintragsarten in TransitionArray:
//
//   - Default-Action für Event
//
//     * für ein Event in einem beliebigen Zustand parametriert eine oder
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
//     * für ein Event in einem beliebigen Zustand parametriert eine oder
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

template <typename Context>
class StateMachineTransitionTable {
  using State_t = typename Context::State_t;
  using Event_t = typename Context::Event_t;
  using Action_t = typename Context::Action_t;
  using Guard_t = typename Context::Guard_t;

  // Konfigurationselement für Transitionen
  // Wird benutzt um die Transitionen von State-Machine zu parametrieren.
  using Transition_t = StateMachineTransition<Context>;

  // Transitionsinformation für {Source-State,Event}-Paar.
  using TransitionForStateAndEvent_t = StateMachineTransitionData<Context>;

  // Um Konfiguration von State-Machine zu vereinfachen werden alle
  // Transitionen als eine Datenstruktur von Typ Transition definiert.
  // Mit TransitionType, wird die Art der Transition vermerkt.
  using TransitionType_t = typename Transition_t::TransitionType;

  template <size_t N>
  using TransitionArray = Transition_t[N];

 public:
  // Konstruktor: Initialisierung der TransitionMap-Instanz nur mit der
  // Transition-Konfiguration
  template <size_t TRANSITION_COUNT>
  StateMachineTransitionTable(
      const TransitionArray<TRANSITION_COUNT>& transitions);

  const TransitionForStateAndEvent_t& GetTransition(State_t src_state,
                                                    Event_t event) const;
  virtual ~StateMachineTransitionTable() = default;

 private:
  void SetupTransitions(const Transition_t* transitions,
                        size_t transition_count);

  void SetupDefaultAction(const Transition_t& transition);

  void SetupDefaultTransition(const Transition_t& transition);

  void SetupTransition(const Transition_t& transition);

  void SetupConditionalTransition(const Transition_t& transition);

  struct StateTransitions {
    TransitionForStateAndEvent_t event_transitions[Event_t::kEventCount];
  };

  StateTransitions state_transitions_[State_t::kStateCount];
};

template <typename Context>
template <size_t TRANSITION_COUNT>
StateMachineTransitionTable<Context>::StateMachineTransitionTable(
    const TransitionArray<TRANSITION_COUNT>& transitions) {
  SetupTransitions(transitions, TRANSITION_COUNT);
}

template <typename Context>
void StateMachineTransitionTable<Context>::SetupTransitions(
    const Transition_t* transitions, size_t transition_count) {
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

template <typename Context>
void StateMachineTransitionTable<Context>::SetupDefaultAction(
    const Transition_t& transition) {
  auto event = transition.event;

  int state_idx = 0;
  for (auto& state : state_transitions_) {
    auto& event_transition = state.event_transitions[event];
    event_transition.guard_action = nullptr;
    event_transition.trans1_dst_state = static_cast<State_t>(state_idx);
    event_transition.trans1_action = transition.transition_data.trans1_action;
    event_transition.trans2_dst_state = Context::kInvalidStateId;
    event_transition.trans2_action.SetEmpty();
    ++state_idx;
  }
}

template <typename Context>
void StateMachineTransitionTable<Context>::SetupDefaultTransition(
    const Transition_t& transition) {}

template <typename Context>
void StateMachineTransitionTable<Context>::SetupTransition(
    const Transition_t& transition) {
  TransitionForStateAndEvent_t& event_transition =
      state_transitions_[transition.src_state]
          .event_transitions[transition.event];

  event_transition = transition.transition_data;
}

template <typename Context>
void StateMachineTransitionTable<Context>::SetupConditionalTransition(
    const Transition_t& transition) {}

template <typename Context>
const typename StateMachineTransitionTable<
    Context>::TransitionForStateAndEvent_t&
StateMachineTransitionTable<Context>::GetTransition(State_t src_state,
                                                    Event_t event) const {
  return state_transitions_[src_state].event_transitions[event];
}

}  // namespace aofsm

#endif  // AOFSM_SRC_INTERNAL_STATE_MACHINE_TRANSITION_TABLE_H_
