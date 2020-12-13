// copyright Yevgen
//
// Default-Aktion pro Event in allen Zuständen, in welchen keinen Übergang für
// den Event spezifiziert wird

#include <cstddef>

using std::size_t;

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

  using ClientStateMachine_t = aofsm::StateMachine<SimlpeClient5>;

  ClientStateMachine_t state_machine_{
      this,
      {
          // Default-Actionen
          // Event | Aktion
          {kEvent1, &DoEvent1},
          {kEvent2, &DoEvent2},
          // Transition
          // Src-Zustand | Event | Dst-Zustand | Aktion
          {STATE1, kEvent2, STATE2, &DoEvent2},
          {STATE2, kEvent1, STATE1, &DoEvent1},
      }};

 public:
  void Event1() { state_machine_.Trigger(kEvent1); }
  void Event2() { state_machine_.Trigger(kEvent2); }
};

TEST(aofsm_StateMachine_default_action, instantiate) {
  SimlpeClient5 simple_client;
}
