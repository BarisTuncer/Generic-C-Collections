project(Examples)

add_executable(funwithvectors ${PROJECT_SOURCE_DIR}/funwithvectors.c)
target_compile_options(funwithvectors PUBLIC -g -Wall)
target_link_libraries(funwithvectors PUBLIC vector)

add_executable(funwithstacks ${PROJECT_SOURCE_DIR}/funwithstacks.c)
target_compile_options(funwithstacks PUBLIC -g -Wall)
target_link_libraries(funwithstacks PUBLIC stack)

add_executable(funwithqueues ${PROJECT_SOURCE_DIR}/funwithqueues.c)
target_compile_options(funwithqueues PUBLIC -g -Wall)
target_link_libraries(funwithqueues PUBLIC queue)