
if(ENABLE_TESTING)
    message(STATUS "Test executable for test_vector will be created")
    add_executable(test_vector test_vector.c)
    target_compile_options(test_vector PUBLIC "-fsanitize=address" "-g" "-O1" "-fno-omit-frame-pointer")
    target_compile_definitions(test_vector PUBLIC TESTING)
    target_compile_definitions(test_vector PUBLIC TEST_LOGS_DIR="${PROJECT_SOURCE_DIR}/build")
    target_link_libraries(test_vector asan vector m log cmocka)     # m is for math library
    add_test(NAME test_vector COMMAND test_vector)
    set_tests_properties(test_vector PROPERTIES TIMEOUT 30)

    if(ENABLE_CODE_COVERAGE)
        #[[ now include code coverage
        Example use of code coverage
            set(COVERAGE_EXCLUDES "dir1/*")
            setup_target_for_coverage_gcovr_html(
                NAME coverage
                EXECUTABLE testrunner
                BASE_DIRECTORY "${PROJECT_SOURCE_DIR}/src"
                EXCLUDE "dir2/*")
        ]]
        set(COVERAGE_EXCLUDES "examples/*" "tests/*")
        setup_target_for_coverage_gcovr_html(
            NAME vector_coverage  
            EXECUTABLE test_vector
            BASE_DIRECTORY "${CMAKE_SOURCE_DIR}/libs/src"
        )
        endif()
endif()
