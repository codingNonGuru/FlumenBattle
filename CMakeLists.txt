cmake_minimum_required(VERSION 3.28)

set(CMAKE_BUILD_TYPE None)
#set(CMAKE_CXX_SCAN_FOR_MODULES ON)
#set(CXX_SCAN_FOR_MODULES ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -fpermissive -w -g")

set(CMAKE_CXX_STANDARD 20)

set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/FlumenBattle)

file(GLOB_RECURSE SOURCE_FILES
	"${PROJECT_SOURCE_DIR}/*"
)

include_directories(/home/andrei/Documents/Code/Libraries/nlohmann)
include_directories(/home/andrei/Documents/Code/Libraries/glm)
include_directories(/home/andrei/Documents/Code/Libraries/FlumenCore)
include_directories(/home/andrei/Documents/Code/Libraries/FlumenEngine)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

link_directories(/home/andrei/Documents/Code/Libraries/FlumenCore)
link_directories(/home/andrei/Documents/Code/Libraries/FlumenEngine)

project (Battle)
 
add_executable(Battle ${SOURCE_FILES})

target_link_libraries(Battle GL GLEW pthread SDL2 SDL2_ttf SDL2_image SDL2_mixer Engine Core)

target_compile_definitions(Battle PRIVATE MAXIMUM_WORLD_SIZE=128)
target_compile_definitions(Battle PRIVATE MINIMUM_WORLD_SIZE=32)
target_compile_definitions(Battle PUBLIC TILES_PER_SIMULATION_DOMAIN=4)

add_subdirectory(/home/andrei/Documents/Code/Libraries/FlumenEngine 
	/home/andrei/Documents/Code/Libraries/FlumenEngine)
add_subdirectory(/home/andrei/Documents/Code/Libraries/FlumenCore
	/home/andrei/Documents/Code/Libraries/FlumenCore)