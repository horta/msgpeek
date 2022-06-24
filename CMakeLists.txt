cmake_minimum_required(VERSION 3.16.3 FATAL_ERROR)

project(
  msgpeek
  VERSION 0.0.1
  LANGUAGES C)

# Generate compile_commands.json file
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/compiler-options.cmake)
include(cmake/sanitizers.cmake)
include(cmake/CPM.cmake)

include(cmake/compiler-options.cmake)
include(cmake/sanitizers.cmake)

cpmaddpackage(NAME c_toolbelt VERSION 0.3.1 GITHUB_REPOSITORY horta/c-toolbelt)
cpmaddpackage(NAME lite_pack VERSION 0.3.0 GITHUB_REPOSITORY
              EBI-Metagenomics/lite-pack)

set(FILE_SOURCES src/msgpeek.c)

add_executable(msgpeek src/msgpeek.c)
target_link_libraries(msgpeek PUBLIC C_TOOLBELT::c_toolbelt)
target_link_libraries(msgpeek PUBLIC LITE_PACK::lite_pack)

target_compile_features(msgpeek PRIVATE c_std_11)
target_compile_options(msgpeek PRIVATE ${WARNING_FLAGS})

target_include_directories(
  msgpeek
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)

set(CPACK_PACKAGE_NAME msgpeek)
set(CPACK_PACKAGE_VENDOR "Danilo Horta")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "MessagePack viewer")
set(CPACK_PACKAGE_CONTACT "Danilo Horta")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
set(CPACK_RESOURCE_FILE_README ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
set(CPACK_OUTPUT_FILE_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/package)
set(CPACK_VERBATIM_VARIABLES YES)
set(CPACK_PACKAGE_RELOCATABLE YES)
set(CPACK_MONOLITHIC_INSTALL YES)
include(CPack)
