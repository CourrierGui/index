cmake_minimum_required(VERSION 3.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(${CMAKE_VERSION} VERSION_LESS 3.17)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.17)
endif()

project(index
    VERSION 0.1
    DESCRIPTION "Simple file indexer"
    LANGUAGES CXX
)

# set(CMAKE_CXX_COMPILER /usr/bin/clang++)
# set(CMAKE_CXX_CPPCHECK "cppcheck")
# set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=*")
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

find_package(SQLiteCpp REQUIRED)

###########################
#### Compilation flags ####
###########################

add_library(compilation_flags INTERFACE)
target_compile_options(compilation_flags
  INTERFACE
      -Wall -Wextra -Wpedantic
)
target_compile_features(compilation_flags INTERFACE cxx_std_20)

#####################
####    Tests    ####
#####################

option(BUILD_TESTING "Build the tests" ON)
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
  enable_testing()
  add_subdirectory(externs/clean-test)
  add_subdirectory(tests)
endif()

#################
##### BUILD #####
#################

add_library(index_utils STATIC
    include/cli.hpp
    src/cli.cpp
    include/db.hpp
    src/db.cpp
)
target_link_libraries(index_utils
    PRIVATE
        compilation_flags
        SQLiteCpp
)
target_include_directories(index_utils PUBLIC include)

add_executable(index index.cpp)
target_link_libraries(index compilation_flags index_utils)
install(
    TARGETS index
    RUNTIME DESTINATION bin
)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_subdirectory(docs)
endif()
