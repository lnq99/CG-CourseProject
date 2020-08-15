cmake_minimum_required(VERSION 3.7)
# set(CMAKE_CXX_COMPILER g++-10)

project(RayTracing)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include_directories(lib/imgui)
# include_directories(lib/glm)
include_directories(vkbase)


# file(GLOB SOURCE *.cpp)
# add_definitions(-DNDEBUG)
add_definitions(-DVK_DATA_DIR=\"${CMAKE_SOURCE_DIR}/data/\")


set(LIB_DIR "${CMAKE_SOURCE_DIR}/lib/bin/")
set(Vulkan_LIBRARY  "${LIB_DIR}/libvulkan.so")
set(XCB_LIBRARY     "${LIB_DIR}/libxcb.so")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_XCB_KHR")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3")

message(STATUS ${Vulkan_LIBRARY})
message(STATUS ${XCB_LIBRARY})

# already link in libvkbase
# link_libraries(
#     ${Vulkan_LIBRARY}
#     ${XCB_LIBRARY}
# )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../")

add_subdirectory(vkbase)
add_subdirectory(src)
