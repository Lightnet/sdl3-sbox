cmake_minimum_required(VERSION 3.20)
project(ValveNetworkTest)


set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "")
set(VCPKG_MANIFEST_MODE ON)
# find_package(GameNetworkingSockets CONFIG REQUIRED)
find_package(gamenetworkingsockets CONFIG REQUIRED)

add_executable(${PROJECT_NAME} 
  # main.cpp
  # vsockettest.cpp
  networkinputtest.cpp
)
# MESSAGE(STATUS "DIR: ${gamenetworkingsockets_INCLUDE_DIRS}")
# target_include_directories(${PROJECT_NAME} PRIVATE 
#   ${GAMENETWORKINGSOCKETS_INCLUDE_DIRS}
# )

# Use either shared or static
target_link_libraries(${PROJECT_NAME} PRIVATE GameNetworkingSockets::shared)


# find_package(GameNetworkingSockets CONFIG REQUIRED PATHS "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}" NO_DEFAULT_PATH)
# find_package(GameNetworkingSockets CONFIG REQUIRED 
#     PATHS "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/share/gamenetworkingsockets"
#     NO_DEFAULT_PATH)

# find_package(GameNetworkingSockets CONFIG REQUIRED 
#     PATHS "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg_installed/x64-windows/share/gamenetworkingsockets"
#     NO_DEFAULT_PATH)

# target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)