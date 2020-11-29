add_executable(aofsm_tst 
               state_machine_tst.cpp)
           
target_link_libraries(aofsm_tst PUBLIC gmock_main
                                PRIVATE aofsm )