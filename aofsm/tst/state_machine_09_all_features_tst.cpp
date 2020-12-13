// copyright Yevgen
//
// Alle State-Machine Parametrierungsmöglichkeiten in einem Client.
//

#include <cstddef>

using std::size_t;

#include "aofsm/src/state_machine.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

// Beispiel Client Class für State-Machine
class SimlpeClient9 {
 public:
  // Wenn nichts anderes parametriert, erwartet State-Machine bei dem
  // Client-Class folgende enums:
  //
  // enum State, mit ...
  //
  //  - Konstanten für jeden Zustand
  //
  //  - kStateCount-Konstante - maximale Anzahl an Zuständen
  //
  //  - INITIAL_STATE-Konstante - Zustand nach der Instanziierung
  //
  enum State {
    ONE_STATE,
    OTHER_STATE,
    OTHER_STATE_1,
    OTHER_STATE_2,
    kStateCount,
    INITIAL_STATE = ONE_STATE
  };
  //
  // enum Event, mit ...
  //
  //  - Konstanten für jeden Event
  //
  //  - kStateCount-Konstante - maximale Anzahl an Events
  //
  enum Event { kEvent1, kEvent2, kEvent3, kEventCount };

  // Wenn nichts anderes parametriert, haben alle Actions, welche bei
  // Zustandsübergängen aufgerufen werden, Signatur void(void).
  void DoEvent1() {}

  void DoEvent1InInitialState() {}

  void DoEvent2() {}

  void DoEvent2InOtherState() {}

  void DoEvent3() {}

  void DoEvent3InInitialState() {}

  void DoEvent3InOtherState() {}

  void DoOnOtherStateEntry() {}
  void DoOnOtherStateEntry2() {}

  void DoOnOtherStateExit() {}
  void DoOnOtherStateExit2() {}

  // Im einfachsten Fall muss StateMachine-Template mit der Client Class
  // parametriert werden.
  using ClientStateMachine_t = aofsm::StateMachine<SimlpeClient9>;

  // Bei Instantiierung muss Zeiger auf Client Class-Instanz übergeben
  // werden.
  //
  // Zweites Parameter: Array von Transition-Entries
  ClientStateMachine_t state_machine_{
      this,
      {
          // Default-Actionen
          //
          // Event | Aktion
          {kEvent1, &DoEvent1},  ///< Wenn weiter nichts anderes
                                 ///< spezifiziert, dann wird bei kEvent1 in
                                 ///< allen Zuständen DoEvent1 aufgerufen,
                                 ///< der Zustand wird nicht gewechselt.

          // Default-Transitionen, können Default-Actionen überschreiben
          //
          // Event | Zielzustand | Aktion
          {kEvent2, INITIAL_STATE,
           &DoEvent2},  ///< Wenn weiter nichts anderes Spezifiziert
                        ///< wird bei kEvent2 in allen
                        ///< Zuständen DoEvent2 aufgerufen, der
                        ///< Zustand wird zu INITIAL_STATE gewechselt.

          // Transitionen, können Default-Actionen und -Transitionen
          // überschreiben.
          // Wenn im Zusand Source-State das Event getriggert wird:
          //   Wehchsel in Dest-State
          //   und Ausführung von Actions.
          //
          // Source-State |  Event | Dest-State | Actions
          {ONE_STATE,
           kEvent3,
           OTHER_STATE,
           {&DoEvent3, &DoEvent3InInitialState}},
          {OTHER_STATE, kEvent3, ONE_STATE, {&DoEvent3, &DoEvent3InOtherState}},

          // überschreibt Default-Aktion für kEvent1 in INITIAL_STATE
          {ONE_STATE, kEvent1, ONE_STATE, {&DoEvent1, &DoEvent1InInitialState}},

          // überschreibt Default-Transition für kEvent2 in OTHER_STATE
          {OTHER_STATE, kEvent2, ONE_STATE, {&DoEvent2, &DoEvent2InOtherState}},
      },
      {
          // Parametrierung der Zuständen
          {OTHER_STATE, &DoOnOtherStateEntry, &DoOnOtherStateExit,
           OTHER_STATE_1,
           OTHER_STATE_2},  ///< OnEntry und OnExit, 2 Sub-Zustände
          {OTHER_STATE, {}, &DoOnOtherStateExit},   ///< nur OnExit
          {OTHER_STATE, &DoOnOtherStateEntry, {}},  ///< nur OnEntry
          {OTHER_STATE,
           {&DoOnOtherStateEntry, &DoOnOtherStateEntry2},
           {&DoOnOtherStateExit, &DoOnOtherStateExit2},
           OTHER_STATE_1,
           OTHER_STATE_2},  ///< OnEntry und OnExit
      }};

 public:
  void Event1() { state_machine_.Trigger(kEvent1); }
  void Event2() { state_machine_.Trigger(kEvent2); }
  void Event3() { state_machine_.Trigger(kEvent3); }
};

TEST(aofsm_StateMachine, instantiate) { SimlpeClient9 simple_client; }
