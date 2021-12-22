// copyright Yevgen

#ifndef AOFSM_SRC_STATE_MACHINE_V1_STATE_MACHINE_DESCRIPTION_H_
#define AOFSM_SRC_STATE_MACHINE_V1_STATE_MACHINE_DESCRIPTION_H_

#include "aofsm/src/std_types.h"

#include "aofsm/src/internal/transition_data.h"

#include "aofsm/src/internal/state_info.h"

#include "aofsm/src/internal/transition.h"

namespace aofsm {

// Die Klasse StateMachineDescription erlaubt es einem Client-Class aus einer
// Menge von erlaubten Statemachine-Transitionen {SrcState,  Event, DstState,
// Action} einen Map zu erzeugen:
//
//    {SrcState,Event} => {DstState,Action}
//
// Die Events und Zustände müssen im Client-Class als Enums definiert sein.
//
// Die Statemachine-Transitionen können als TransitionArray als
// Kosntruktor-Parameter übergeben werden.
//
// Beispiel:
//
//  Für folgende Statemachine
//  - zwei States: StateA, StateB
//  - ein Event: EventAction.
//  - zwei Actions: DoA DoB
//  - zwei Transitionen:
//    -  StateA EventAction/DoA -> StateA
//    -  StateB EventAction/DoB -> StateB
//
//  muss der Clien-Class
//
//  - zwei nested Enums haben: :
//      -  enum State { kStateA, kStateB, kStateCount};
//      -  enum Event { kEventAction , kEventCount };
//
//  - statisch StateMachineDescription instanziiern mit folgendem
//    TransitionArray als Konstruktor-Parameter :
//
//     {{kStateA, kEventAction, kStateA, &Client::DoA},
//      {kStateB, kEventAction, kStateB, &Client::DoB}}
//
// Allgemeine Beschreibung der Schnittstellle:
//
//  Die StateMachineDescription ist ein Template mit folgenden Parametern:
//  - State_t : Datentyp für State-Id, muss enum sein und folgende Konstante
//              kStateCount haben
//              kStateCount - Anzahl der Zustände
//  - Event_t : Datentyp für Event, muss enum sein und folgende Konstante
//              kEvent haben
//              kEvent - Anzahl der Events
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

template <typename Context>
class StateMachineDescription {
 public:
  using State_t = typename Context::State_t;
  using Event_t = typename Context::Event_t;
  using Action_t = typename Context::Action_t;
  using Guard_t = typename Context::Guard_t;

  // Konfigurationselement für Transitionen
  // Wird benutzt um die Transitionen von State-Machine zu parametrieren.
  using Transition_t = Transition<Context>;

  // Transitionsinformation für {Source-State,Event}-Paar.
  using TransitionForStateAndEvent_t = TransitionData<Context>;

  // Um Konfiguration von State-Machine zu vereinfachen werden alle
  // Transitionen als eine Datenstruktur von Typ Transition definiert.
  // Mit TransitionType, wird die Art der Transition vermerkt.
  using TransitionType_t = typename Transition_t::TransitionType;

  template <size_t N>
  using TransitionArray = Transition_t[N];

  using StateInfo_t = StateInfo<State_t, Action_t>;

  template <size_t N>
  using StateInfoArray = StateInfo_t[N];

  // Konstruktor: Initialisierung der TransitionMap-Instanz nur mit der
  // Transition-Konfiguration
  template <size_t TRANSITION_COUNT>
  StateMachineDescription(const TransitionArray<TRANSITION_COUNT>& transitions);

  // Konstruktor: Initialisierung der State-Machine mit der
  // Transition-Konfiguration und State-Konfiguration
  template <size_t TRANSITION_COUNT, size_t STATE_INFO_COUNT>
  StateMachineDescription(const TransitionArray<TRANSITION_COUNT>& transitions,
                          const StateInfoArray<STATE_INFO_COUNT>& state_infos);

  virtual ~StateMachineDescription() = default;

  const TransitionForStateAndEvent_t& GetTransition(State_t src_state,
                                                    Event_t event) const;

 private:
  void SetupTransitions(const Transition_t* transitions,
                        size_t transition_count);

  void SetupDefaultAction(const Transition_t& transition);

  void SetupDefaultTransition(const Transition_t& transition);

  void SetupTransition(const Transition_t& transition);

  void SetupConditionalTransition(const Transition_t& transition);

  void SetupStates(const StateInfo_t* state_infos, size_t state_info_count);

  void SetupSubStates(State_t state, const State_t* substates,
                      size_t substates_count);

  struct StateTransitions {
    State_t parent_state{Context::kInvalidStateId};

    Action_t on_entry_action;

    Action_t on_exit_action;

    TransitionForStateAndEvent_t event_transitions[Event_t::kEventCount];
  };

  StateTransitions state_transitions_[State_t::kStateCount];
};

template <typename Context>
template <size_t TRANSITION_COUNT>
StateMachineDescription<Context>::StateMachineDescription(
    const TransitionArray<TRANSITION_COUNT>& transitions) {
  SetupTransitions(transitions, TRANSITION_COUNT);
}

template <typename Context>
template <size_t TRANSITION_COUNT, size_t STATE_INFO_COUNT>
StateMachineDescription<Context>::StateMachineDescription(
    const TransitionArray<TRANSITION_COUNT>& transitions,
    const StateInfoArray<STATE_INFO_COUNT>& state_infos) {
  SetupTransitions(transitions, TRANSITION_COUNT);
  SetupStates(state_infos, STATE_INFO_COUNT);
}

template <typename Context>
void StateMachineDescription<Context>::SetupTransitions(
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
void StateMachineDescription<Context>::SetupStates(
    const StateInfo_t* state_infos, size_t state_info_count) {
  auto end = state_infos + state_info_count;
  for (auto state_info = state_infos; state_info != end; ++state_info) {
    auto& state = state_transitions_[state_info->state];
    state.on_entry_actions = state_info->on_entry_actions;
    state.on_exit_actions = state_info->on_exit_actions;
    SetupSubStates(state_info->state, state_info->sub_states,
                   state_info->sub_states_count);
  }
}

template <typename Context>
void StateMachineDescription<Context>::SetupDefaultAction(
    const Transition_t& transition) {
  auto event = transition.event;

  int state_idx = 0;
  for (auto& state : state_transitions_) {
    auto& event_transition = state.event_transitions[event];
    event_transition.guard_action = nullptr;
    event_transition.trans1_dst_state = static_cast<State_t>(state_idx);
    event_transition.trans1_actions = transition.transition_data.trans1_actions;
    event_transition.trans2_dst_state = Context::kInvalidStateId;
    event_transition.trans2_actions.SetEmpty();
    ++state_idx;
  }
}

template <typename Context>
void StateMachineDescription<Context>::SetupDefaultTransition(
    const Transition_t& transition) {}

template <typename Context>
void StateMachineDescription<Context>::SetupTransition(
    const Transition_t& transition) {
  TransitionForStateAndEvent_t& event_transition =
      state_transitions_[transition.src_state]
          .event_transitions[transition.event];

  event_transition = transition.transition_data;
}

template <typename Context>
void StateMachineDescription<Context>::SetupConditionalTransition(
    const Transition_t& transition) {}

template <typename Context>
void StateMachineDescription<Context>::SetupSubStates(State_t state,
                                                      const State_t* substates,
                                                      size_t substates_count) {}

template <typename Context>
const typename StateMachineDescription<Context>::TransitionForStateAndEvent_t&
StateMachineDescription<Context>::GetTransition(State_t src_state,
                                                Event_t event) const {
  return state_transitions_[src_state].event_transitions[event];
}

}  // namespace aofsm

#endif  // AOFSM_SRC_STATE_MACHINE_V1_STATE_MACHINE_DESCRIPTION_H_
