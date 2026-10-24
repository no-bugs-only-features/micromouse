add_executable(mousetrace_server main.c server.c)

target_compile_features(mousetrace_server PRIVATE c_std_17)

# tell the c server where to find packetstructure.h
target_include_directories(mousetrace_server PRIVATE ${CMAKE_SOURCE_DIR}/shared)


