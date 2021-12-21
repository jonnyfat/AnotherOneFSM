// Copyright Yevgen

#ifndef AOFSM_SRC_TRANSITION_H_
#define AOFSM_SRC_TRANSITION_H_

#include "transition_data.h"

namespace aofsm {

// Struktur für Transitionen einer State-Machine
template <typename Context>
struct Transition {
  using State_t = typename Context::State_t;
  using Event_t = typename Context::Event_t;
  using Action_t = typename Context::Action_t;
  using Guard_t = typename Context::Guard_t;

  // Um Konfiguration von State-Machine zu vereinfachen werden alle
  // Transitionen als eine Datenstruktur von Typ Transition definiert. Mit
  // TransitionType, wird die Art der Transition vermerkt.
  enum class TransitionType {
    kDefaultAction,  ///< Default-Aktion für ein Event in beliebigen
                     ///< Zustand. Nur das Event und action sind angegeben.
                     ///< Gilt für alle Zustände in welchen für das Event
                     ///< keine Default-Transitionen und keine Transition
                     ///< festgelegt sind.

    kDefaultTransition,  ///< Default-Transition für ein Event aus
                         ///< beliebigen Zustand fürhrt in einen anderen
                         ///< Zustand. Nur Event, Zielzustand und action
                         ///< sind angegeben. Gilt für alle Zustände in
                         ///< welchen für das Event keine Transition
                         ///< festgelegt ist.

    kTransition,  ///< Transition für ein Event in einem Zustand führt in
                  ///< einen anderen Zustand. Gilt für alle Zustände für
                  ///< welche keine ConditionalTransition festgelegt ist.

    kConditionalTransition  ///< Bedingte Transition für ein Event in einem
                            ///< Zustand.
                            ///< Enthält ein Guard und zwei Transitionen.
                            ///< Bei Triggerung wird der Guard aufgerufen
                            ///< und je nach Rückgabewert wird entweder eine
                            ///< oder andere Transaktion durchgeführt.
  };

  const TransitionType transition_type;

  const State_t src_state;  ///< nur bei kTransition und kConditionalTransition

  const Event_t event;  ///< für alle TransitionType

  TransitionData<Context> transition_data;

  // Konstruktor für Parametrieren von Default-Action für ein Event der
  // State-Machine. Es gilt für alle Zustädnde der Machine für welche keine
  // Default-Transition und kein Transition für das Event parametriert ist.
  Transition(Event_t event, const Action_t& action)
      : transition_type{TransitionType::kDefaultAction},
        src_state{Context::kInvalidStateId},        ///<  ungültig
        event{event},                               ///< gültig
        transition_data{nullptr,                   ///<  kein Guard
                         Context::kInvalidStateId,  ///<  ungültig
                         action,                    ///< gültig
                         Context::kInvalidStateId,  ///<  ungültig
                         Action_t()} {}             ///<  leer

  // Konstruktor für Parametrierung einer Default-Transition für ein Event der
  // State-Machine. Es gilt für alle Zustände der Machine für welche keine
  // Transition für das Event parametriert ist.
  Transition(Event_t event, State_t dst_state, const Action_t& action)
      : transition_type{TransitionType::kDefaultTransition},
        src_state{Context::kInvalidStateId},        ///<  ungültig
        event{event},                               ///< gültig
        transition_data{nullptr,                   ///<  kein Guard
                         dst_state,                 ///< gültig
                         action,                    ///< gültig
                         Context::kInvalidStateId,  ///<  ungültig
                         Action_t()} {}             ///<  leer

  // Konstruktor für Parametrierung einer unbedingten Transition für ein Event
  // in einem Zustand.
  Transition(State_t src_state, Event_t event, State_t dst_state,
             const Action_t& action)
      : transition_type{TransitionType::kTransition},
        src_state{src_state},
        event{event},
        transition_data{nullptr, dst_state, action, Context::kInvalidStateId,
                         Action_t()} {}

  // Konstruktor für Parametrierung einer bedingten Transition für ein Event
  // in einem Zustand.
  Transition(State_t src_state, Event_t event, Guard_t guard_action,
             State_t dst_state_1, const Action_t& action_1, State_t dst_state_2,
             const Action_t& action_2)
      : transition_type{TransitionType::kConditionalTransition},
        src_state{src_state},
        event{event},
        transition_data{guard_action, dst_state_1, action_1, dst_state_2,
                         action_2} {}
};

}  // namespace aofsm

#endif  // AOFSM_SRC_TRANSITION_H_
