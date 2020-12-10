// copyright Yevgen

#include <cstddef>

using std::size_t;

#include "aofsm/src/state_machine.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

class SimlpeClient {
 public:
  enum State { IDLE_STATE, ACTIVE_STATE, kStateCount, INITIAL_STATE = IDLE_STATE };
  enum Event { kActivateEvt, kDeactivateEvt, kTickEvt, kEventCount };

  void Activate() { state_machine_.Trigger(kActivateEvt); }
  void Deactivate() { state_machine_.Trigger(kDeactivateEvt); }
  void Tick() { state_machine_.Trigger(kTickEvt); }

 private:
  void DoActivate() {}
  void DoDeactivate() {}
  void DoNop() {}
  void DoTick() {}

  using ClientStateMachine_t = aofsm::StateMachine<SimlpeClient>;

  ClientStateMachine_t state_machine_{
      this,
      {
          // Default-Action
          {kTickEvt, &DoNop},
          // Default-Transition
          {kDeactivateEvt, IDLE_STATE, &DoDeactivate},
          // Transitions
          {IDLE_STATE, kActivateEvt, ACTIVE_STATE, &DoActivate},
          {ACTIVE_STATE, kTickEvt, ACTIVE_STATE, &DoTick},
      }};
};

TEST(aofsm_StateMachine, instantiate) { SimlpeClient simple_client; }
