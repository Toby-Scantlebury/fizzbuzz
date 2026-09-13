cmake_minimum_required(VERSION 3.20)
project(fizzbuzz LANGUAGES CXX)

include(CTest)

foreach(step RANGE 1 23)
    if(step LESS 10)
        set(step "0${step}")
    endif()
    add_executable(fizzbuzz_${step} fizzbuzz_${step}.cpp)
    set_target_properties(fizzbuzz_${step} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO)
    if(BUILD_TESTING)
        add_test(NAME fizzbuzz_${step}_output
            COMMAND ${CMAKE_COMMAND}
                -DPROGRAM=$<TARGET_FILE:fizzbuzz_${step}>
                -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/check_output.cmake)
    endif()
endforeach()

if(BUILD_TESTING)
    add_executable(sequence_contracts tests/sequence_contracts.cpp)
    set_target_properties(sequence_contracts PROPERTIES
        CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
    add_test(NAME sequence_contracts COMMAND sequence_contracts)
endif()
