add_executable(tests TESTS/si7050/temp/test_si70.cpp)

add_custom_target(run-tests ALL
        COMMAND mbed test -n tests-si7050* -vv
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})