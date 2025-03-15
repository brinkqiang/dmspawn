cmake_minimum_required(VERSION 3.8)

PROJECT(dmspawn)
LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
INCLUDE(cmake/ModuleImport.cmake)
INCLUDE(cmake/ModuleCompileOptions.cmake)
ModuleSetCompileOptions()
ModuleSetWinCompilerFlags()

find_package(Boost COMPONENTS coroutine REQUIRED)

print_package_vars(Boost)

include_directories(${Boost_INCLUDE_DIRS})
ModuleInclude("dmlog" "thirdparty/dmlog")
ExeImport("test" "${Boost_LIBRARIES}")
