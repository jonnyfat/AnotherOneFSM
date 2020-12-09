add_executable(aofsm_tst 
               state_machine_interface_tst.cpp
               state_machine_usability_tst.cpp
               state_machine_action_with_param_tst.cpp
               state_machine_transition_with_guard_tst.cpp )
           
target_link_libraries(aofsm_tst PUBLIC gmock_main
                                PRIVATE aofsm )