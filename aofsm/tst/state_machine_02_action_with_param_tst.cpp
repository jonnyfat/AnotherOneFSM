// copyright Yevgen
//
// Actions bei Transitionen haben Parameter

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "aofsm/src/std_types.h"
#include "aofsm/src/state_machine.h"

// state machine
//
//  Aktionen habe Parameter (Data*)
//
//  [Init]+->(StartAEvt -> [A_STATE]
//        |    \DoStartA(Data*))
//        |                                      |
//        +->(StartBEvt -> [B_STATE]
//             \DoStartB(Data*))
//
//   states :
//            INITIAL_STATE, A_STATE, B_STATE
//   events :
//            kStartAEvt, kStartBEvt
//   actions :
//             DoStartA, DoStartB
//   transitions:
//         INITIAL_STATE -> A_STATE
//            kStartAEvt/DoStartA
//
//         INITIAL_STATE -> B_STATE
//            kStartBEvt/DoStartB
//

class SimlpeClient2 {
 public:
  enum State { INITIAL_STATE, A_STATE, B_STATE, kStateCount };
  enum Event { kStartAEvt, kStartBEvt, kEventCount };

  struct Data {
    char* data{nullptr};
  };

  //  void DoStartA(Data*) {}
  MOCK_METHOD(void, DoStartA, (Data*), ());
  //  void DoStartB(Data*) {}
  MOCK_METHOD(void, DoStartB, (Data*), ());

  DECL_STATE_MACHINE_ACTIONS_WITH_PARAMETERS(SimlpeClient2, state_machine_,
                                             MAX_ACTIONS_PER_TRANSITION(1),
                                             ACTION_PARAMETERS(Data*));
};

DEF_STATE_MACHINE(SimlpeClient2, state_machine_,
                  // Transitions
                  // {Source-State , Event , Destination-State , Action}
                  {INITIAL_STATE, kStartAEvt, A_STATE, &DoStartA},
                  {INITIAL_STATE, kStartBEvt, B_STATE, &DoStartB});

TEST(aofsm_StateMachineActionWithParam,
     MustDoTransitionTo_A_STATE_AndCallAction_DoStartA) {
  // Arrange
  SimlpeClient2 simple_client;
  SimlpeClient2::Data data;
  EXPECT_CALL(simple_client, DoStartA(&data)).Times(1);
  EXPECT_CALL(simple_client, DoStartB).Times(0);

  // Act
  simple_client.state_machine_.Trigger(SimlpeClient2::Event::kStartAEvt, &data);

  // Assert
  EXPECT_EQ(simple_client.state_machine_.GetCurrentState(),
            SimlpeClient2::State::A_STATE);
}

TEST(aofsm_StateMachineActionWithParam,
     MustDoTransitionTo_B_STATE_AndCallAction_DoStartB) {
  // Arrange
  SimlpeClient2 simple_client;
  SimlpeClient2::Data data;
  EXPECT_CALL(simple_client, DoStartA).Times(0);
  EXPECT_CALL(simple_client, DoStartB(&data)).Times(1);

  // Act
  simple_client.state_machine_.Trigger(SimlpeClient2::Event::kStartBEvt, &data);

  // Assert
  EXPECT_EQ(simple_client.state_machine_.GetCurrentState(),
            SimlpeClient2::State::B_STATE);
}
