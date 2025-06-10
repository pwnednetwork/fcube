cmake_minimum_required(VERSION 3.16)
project(TrigramVoxelViewer)

set(CMAKE_CXX_STANDARD 17)

# Find packages
find_package(PkgConfig REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)

# Find GLM
find_path(GLM_INCLUDE_DIR glm/glm.hpp)

# Find nlohmann/json
find_package(nlohmann_json REQUIRED)

# Create executable
add_executable(${PROJECT_NAME} src/main.cpp)

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    glfw 
    Vulkan::Vulkan
    nlohmann_json::nlohmann_json
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${GLM_INCLUDE_DIR})

# Copy shaders to build directory
file(COPY ${CMAKE_SOURCE_DIR}/shaders DESTINATION ${CMAKE_BINARY_DIR})
