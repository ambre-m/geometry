cmake_minimum_required(VERSION 3.18)

project(geometry
	VERSION 1.0.0
	LANGUAGES CXX)

option(GEOMETRY_SAMPLES OFF)
option(GEOMETRY_SAMPLES_WITH_SFML OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(geometry INTERFACE)

target_include_directories(geometry
	INTERFACE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/public>
		$<INSTALL_INTERFACE:include>  # <prefix>/include/mylib
	# PRIVATE
		# include/private
)

if(GEOMETRY_SAMPLES)
	if (GEOMETRY_SAMPLES_WITH_SFML)
		find_package(SFML COMPONENTS window graphics system)
	endif()
	add_subdirectory(samples)
endif()

