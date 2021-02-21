project(Examples)

add_executable(funwithvectors ${PROJECT_SOURCE_DIR}/funwithvectors.c)
target_compile_options(funwithvectors PUBLIC -g -Wall)
target_link_libraries(funwithvectors PUBLIC vector)
