project(Extlibs)

add_library(log SHARED ${PROJECT_SOURCE_DIR}/src/log.c)
target_include_directories(log PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_definitions(log PUBLIC LOG_USE_COLOR)
