cmake_minimum_required(VERSION 3.8)

SET(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../dmvcpkg/vcpkg/scripts/buildsystems/vcpkg.cmake")

PROJECT(dmspawn)
LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
INCLUDE(cmake/ModuleImport.cmake)
INCLUDE(cmake/ModuleCompileOptions.cmake)
ModuleSetCompileOptions()
ModuleSetWinCompilerFlags()

find_package(Boost COMPONENTS coroutine REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
ModuleInclude("dmlog" "thirdparty/dmlog")
ExeImport("test" "${Boost_LIBRARIES}")
