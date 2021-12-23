//----------------------------------------------------------------------------
// Copyright: HIMA - Paul Hildebrandt GmbH
//
//----------------------------------------------------------------------------

#ifndef AOFSM_SRC_STATE_MACHINE_H_
#define AOFSM_SRC_STATE_MACHINE_H_

#include "aofsm/src/state_machine_instance.h"

// Macros für Deklaration von State-Machine füe eine Client Klasse
//
// In client_class.h:
//
// class ClientClass {
//  ...;
//  DECL_STATE_MACHINE(ClientClass, state_machine_,
//                     MAX_ACTIONS_PER_TRANSITION(1));
//
//  // oder
//
//  DECL_STATE_MACHINE_ACTIONS_WITH_PARAMETERS(ClientClass, state_machine_,
//                     MAX_ACTIONS_PER_TRANSITION(1),
//                     ACTION_PARAMETERS(Data*));
// };
//
// In client_class.cpp:
//
// DEF_STATE_MACHINE(ClientClass, state_machine_,
//     {INITIAL_STATE, kStartAEvt, A_STATE, &DoStartA},
//     {INITIAL_STATE, kStartBEvt, B_STATE, &DoStartB},
//     {A_STATE, kEndEvt, FINAL_STATE, &DoEndA},
//     {B_STATE, kEndEvt, FINAL_STATE, &DoEndB});

// Macros für Deklaration von State-Machine im Rumpf der Klasse

// Helper-Macro für 3. Parameter für folgende Macros
// - DECL_STATE_MACHINE()
// - DECL_STATE_MACHINE_ACTIONS_WITH_PARAMETERS
#define MAX_ACTIONS_PER_TRANSITION(__action_count__) __action_count__

#define DECL_STATE_MACHINE(__class_name__, __member_name__, __action_count__) \
  friend class aofsm::StateMachineInstance<__class_name__, __action_count__>; \
  using StateMachine_t =                                                      \
      aofsm::StateMachineInstance<__class_name__, __action_count__>;          \
  using TransitionTable_t = StateMachine_t::TransitionTable_t;                \
  static const TransitionTable_t __member_name__##transition_table_;          \
  StateMachine_t __member_name__{this, __member_name__##transition_table_};

// Helper-Macro für 4. Parameter für folgende Macro
// - DECL_STATE_MACHINE_ACTIONS_WITH_PARAMETERS
#define ACTION_PARAMETERS(...) __VA_ARGS__

#define DECL_STATE_MACHINE_ACTIONS_WITH_PARAMETERS(                           \
    __class_name__, __member_name__, __action_count__, __action_parameters__) \
  friend class aofsm::StateMachineInstance<__class_name__, __action_count__,  \
                                           __action_parameters__>;            \
  using StateMachine_t =                                                      \
      aofsm::StateMachineInstance<__class_name__, __action_count__,           \
                                  __action_parameters__>;                     \
  using TransitionTable_t = StateMachine_t::TransitionTable_t;                \
  static const TransitionTable_t __member_name__##transition_table_;          \
  StateMachine_t __member_name__{this, __member_name__##transition_table_};

// Macro für Definition von State-Machine im cpp-Datei der Klasse
#define DEF_STATE_MACHINE(__class_name__, __member_name__, ...) \
  const __class_name__::TransitionTable_t                       \
      __class_name__::__member_name__##transition_table_{{__VA_ARGS__}};

#endif  // AOFSM_SRC_STATE_MACHINE_H_
