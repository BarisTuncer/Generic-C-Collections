project(Libs)

add_library(vector SHARED ${PROJECT_SOURCE_DIR}/src/vector.c)
target_include_directories(vector PUBLIC ${PROJECT_SOURCE_DIR}/include)

