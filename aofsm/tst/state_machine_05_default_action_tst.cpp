// copyright Yevgen
//
// Default-Aktion pro Event in allen Zuständen, in welchen keinen Übergang für
// den Event spezifiziert wird

#include "aofsm/src/std_types.h"



#include "aofsm/src/state_machine.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

// Beispiel Client Class für State-Machine
class SimlpeClient5 {
 public:
  enum State { STATE1, STATE2, kStateCount, INITIAL_STATE = STATE1 };
  enum Event { kEvent1, kEvent2, kEventCount };

 private:
  void DoEvent1() {}

  void DoEvent2() {}

  using StateMachine_t = aofsm::StateMachine<SimlpeClient5>;

  using StateMachineDescription_t = StateMachine_t::StateMachineDescription_t;

  static const StateMachineDescription_t state_machine_description_;

  StateMachine_t state_machine_{this, state_machine_description_};

 public:
  void Event1() { state_machine_.Trigger(kEvent1); }
  void Event2() { state_machine_.Trigger(kEvent2); }
};

const SimlpeClient5::StateMachineDescription_t
    SimlpeClient5::state_machine_description_{{
        // Default-Actionen
        // Event | Aktion
        {kEvent1, &DoEvent1},
        {kEvent2, &DoEvent2},
        // Transition
        // Src-Zustand | Event | Dst-Zustand | Aktion
        {STATE1, kEvent2, STATE2, &DoEvent2},
        {STATE2, kEvent1, STATE1, &DoEvent1},
    }};

TEST(aofsm_StateMachine_default_action, instantiate) {
  SimlpeClient5 simple_client;
}
