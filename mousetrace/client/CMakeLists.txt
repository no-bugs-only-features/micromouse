add_executable(mousetrace_client main.cpp)

# set the c++ standard
target_compile_features(mousetrace_client PRIVATE cxx_std_20)

target_include_directories(mousetrace_client PRIVATE ${CMAKE_SOURCE_DIR}/shared)

# link the graphics and the ui libraries
target_link_libraries(mousetrace_client PRIVATE SFML::Graphics ImGui-SFML::ImGui-SFML)
