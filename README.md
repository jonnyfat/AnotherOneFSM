# AnotherOneFSM
Another one implementation of simple State Machine for a Class in C++


Für Übersetzung von Tests muss mit Google-Test mitkomiliert werden:

Project-Directory/
    - AnotherOneFSM/
    - googletest/
    - CMakeLists.txt

CMakeLists.txt :

    cmake_minimum_required(VERSION 3.14.0)
    project(Dummy)

    addsubdirectory( googletest EXCLUDE_FROM_ALL)
    add_subdirectory( AnotherOneFSM)




