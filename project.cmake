add_custom_target(run-tests ALL
        COMMAND mbed test -n tests-si7050* #-vv
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})