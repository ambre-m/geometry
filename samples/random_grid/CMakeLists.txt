
add_custom_target(all_random_grids)

add_executable(fixed_grid fixed_grid.cpp)
target_link_libraries(fixed_grid PRIVATE geometry)
add_dependencies(all_random_grids fixed_grid)

add_executable(random_grid_cli random_grid_cli.cpp)
target_link_libraries(random_grid_cli PRIVATE geometry)
add_dependencies(all_random_grids random_grid_cli)

if(GEOMETRY_SAMPLES_WITH_SFML)
add_executable(random_grid random_grid.cpp)
target_link_libraries(random_grid PRIVATE geometry sfml-graphics)
add_dependencies(all_random_grids random_grid)

add_executable(random_color_grid random_color_grid.cpp)
target_link_libraries(random_color_grid PRIVATE geometry sfml-graphics)
add_dependencies(all_random_grids random_color_grid)


add_executable(random_system random_system.cpp)
target_link_libraries(random_system PRIVATE geometry sfml-graphics)
add_dependencies(all_random_grids random_system)

file(CREATE_LINK ${CMAKE_CURRENT_SOURCE_DIR}/sansation.ttf ${CMAKE_CURRENT_BINARY_DIR}/sansation.ttf SYMBOLIC)
endif()
