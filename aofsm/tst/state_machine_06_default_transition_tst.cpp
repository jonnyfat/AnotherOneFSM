// copyright Yevgen
//
// Default-Tranition pro Event in allen Zuständen, in welchen keinen Übergang
// für den Event spezifiziert wird
//

#include <cstddef>

using std::size_t;

#include "aofsm/src/state_machine.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

// Beispiel Client Class für State-Machine
class SimlpeClient6 {
 public:
  enum State {
    STATE1,
    STATE2,
    ERROR_STATE,
    kStateCount,
    INITIAL_STATE = STATE1
  };
  enum Event { kEvent1, kEvent2, kErrorEvt, kEventCount };

 private:
  void DoEvent1() {}

  void DoEvent2() {}

  void DoError() {}

  using StateMachine_t = aofsm::StateMachine<SimlpeClient6>;

  using StateMachineDescription_t = StateMachine_t::StateMachineDescription_t;

  static const StateMachineDescription_t state_machine_description_;

  StateMachine_t state_machine_{this, state_machine_description_};

 public:
  void Event1() { state_machine_.Trigger(kEvent1); }
  void Event2() { state_machine_.Trigger(kEvent2); }
};

const SimlpeClient6::StateMachineDescription_t
    SimlpeClient6::state_machine_description_{{
        // Default-Transition
        // Event | Ziel-Zustand | Aktion
        {kErrorEvt, ERROR_STATE, &DoError},

        // Transition
        // Src-Zustand | Event | Dst-Zustand | Aktion
        {STATE1, kEvent1, STATE1, &DoEvent1},
        {STATE1, kEvent2, STATE2, &DoEvent2},
        {STATE2, kEvent1, STATE1, &DoEvent1},
        {STATE2, kEvent2, STATE2, &DoEvent2},
    }};

TEST(aofsm_StateMachine_default_transition, instantiate) {
  SimlpeClient6 simple_client;
}
