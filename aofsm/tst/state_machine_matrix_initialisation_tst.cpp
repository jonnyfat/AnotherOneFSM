// Copyright Yevgen
#include <cstddef>

using std::size_t;

#include "gmock/gmock.h"
#include "gtest/gtest.h"

template <typename StateList, typename EventList>
struct Dummy {};

template <size_t N>
struct Transition {
  size_t state;
  size_t data[N];
};

template <size_t Event>
struct TransitionInitValue {
  static constexpr size_t value = Event;
};

template <size_t N, size_t State, size_t Event>
struct TransitionInitValue1 {
  static constexpr size_t data[N] = {};
};

template <size_t N>
struct Transition1 {
  size_t data[N];
};

template <template <typename...> typename StateList,
          template <typename...> typename EventList, typename... States,
          typename... Events>
struct Dummy<StateList<States...>, EventList<Events...>> {
  enum : size_t {
    kStateCount = sizeof...(States),
    kEventCount = sizeof...(Events),
  };

  static constexpr Transition<sizeof...(Events)>
      transitions[sizeof...(States)] = {
          {States::value, {TransitionInitValue<Events::value>::value...}}...};

  static constexpr size_t tmp_data[sizeof...(Events)] = {11, 22, 33};

  static constexpr Transition1<sizeof...(Events)> trans1[sizeof...(States)] = {
      {11, 22, 33}};
};

template <template <typename...> typename StateList,
          template <typename...> typename EventList, typename... States,
          typename... Events>
constexpr Transition<sizeof...(Events)> Dummy<
    StateList<States...>, EventList<Events...>>::transitions[sizeof...(States)];

template <template <typename...> typename StateList,
          template <typename...> typename EventList, typename... States,
          typename... Events>
constexpr size_t Dummy<StateList<States...>,
                       EventList<Events...>>::tmp_data[sizeof...(Events)];

template <template <typename...> typename StateList,
          template <typename...> typename EventList, typename... States,
          typename... Events>
constexpr Transition1<sizeof...(Events)> Dummy<
    StateList<States...>, EventList<Events...>>::trans1[sizeof...(States)];

template <typename... Types>
struct TypeList;

enum State : size_t {
  State1 = 99,
  State2 = 1010,
  State3 = 43,
  State4 = 65,
};

enum Event : size_t {
  Event1 = 205,
  Event2 = 450,
  Event3 = 18290,
};

template <typename T, T v>
struct EnumType {
  static constexpr T value = v;
};

template <typename T, T... vs>
using EnumTypeList = TypeList<EnumType<T, vs>...>;

using StateTypeList = EnumTypeList<State, State1, State2, State3, State4>;

using EventTypeList = EnumTypeList<Event, Event1, Event2, Event3>;

using DummyStateEventMatrix = Dummy<StateTypeList, EventTypeList>;

TEST(MatrixInitialisation, Experiment1) {
  DummyStateEventMatrix state_event_matrix;
  EXPECT_EQ(state_event_matrix.kStateCount, 4);
  EXPECT_EQ(state_event_matrix.kEventCount, 3);

  EXPECT_EQ(state_event_matrix.transitions[0].state, State1);
  EXPECT_EQ(state_event_matrix.transitions[0].data[0], Event1);
  EXPECT_EQ(state_event_matrix.transitions[0].data[1], Event2);
  EXPECT_EQ(state_event_matrix.transitions[0].data[2], Event3);
  EXPECT_EQ(state_event_matrix.transitions[1].state, State2);
  EXPECT_EQ(state_event_matrix.transitions[1].data[0], Event1);
  EXPECT_EQ(state_event_matrix.transitions[1].data[1], Event2);
  EXPECT_EQ(state_event_matrix.transitions[1].data[2], Event3);
  EXPECT_EQ(state_event_matrix.transitions[2].state, State3);
  EXPECT_EQ(state_event_matrix.transitions[2].data[0], Event1);
  EXPECT_EQ(state_event_matrix.transitions[2].data[1], Event2);
  EXPECT_EQ(state_event_matrix.transitions[2].data[2], Event3);
  EXPECT_EQ(state_event_matrix.transitions[3].state, State4);
  EXPECT_EQ(state_event_matrix.transitions[3].data[0], Event1);
  EXPECT_EQ(state_event_matrix.transitions[3].data[1], Event2);
  EXPECT_EQ(state_event_matrix.transitions[3].data[2], Event3);
}

TEST(MatrixInitialisation, Experiment2) {
  DummyStateEventMatrix state_event_matrix;
  EXPECT_EQ(state_event_matrix.tmp_data[0], 11);
  EXPECT_EQ(state_event_matrix.tmp_data[1], 22);
  EXPECT_EQ(state_event_matrix.tmp_data[2], 33);

  EXPECT_EQ(state_event_matrix.trans1[0].data[0], 11);
}
