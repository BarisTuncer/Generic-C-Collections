project(Libs)

add_library(vector SHARED ${PROJECT_SOURCE_DIR}/src/vector.c)
target_include_directories(vector PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_library(stack SHARED ${PROJECT_SOURCE_DIR}/src/stack.c)
target_include_directories(stack PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(stack PRIVATE vector)
