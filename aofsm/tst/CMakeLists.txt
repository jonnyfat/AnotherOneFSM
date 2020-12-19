add_executable(aofsm_tst 
               state_machine_01_basics_tst.cpp
#               state_machine_02_action_with_param_tst.cpp
#               state_machine_03_multiple_actions_tst.cpp
#               state_machine_04_transition_with_guard_tst.cpp 
#               state_machine_05_default_action_tst.cpp
#               state_machine_06_default_transition_tst.cpp
#               state_machine_09_all_features_tst.cpp 
            )
           
target_link_libraries(aofsm_tst PUBLIC gmock_main
                                PRIVATE aofsm )