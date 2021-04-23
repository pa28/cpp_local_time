cmake_minimum_required(VERSION 3.13)
project(cpp_local_time VERSION 0.1.11.1)
set(PACKAGE_CONTACT "Richard Buckley <richard.buckley>@ieee.org>")
include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_VERBOSE_MAKEFILE OFF)

INCLUDE(CheckCXXCompilerFlag)

# Generates an RFC2822 timestamp
#
# The following variable is set:
#   RFC2822_TIMESTAMP
function(rfc2822stamp)
    execute_process(COMMAND date -R
            OUTPUT_VARIABLE RESULT)
    set(RFC2822_TIMESTAMP ${RESULT} PARENT_SCOPE)
endfunction()
rfc2822stamp()

set(CPACK_DEBIAN_PACKAGE_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

# generate a Debian compliant changelog
set(CHANGELOG_HEADER "${PROJECT_NAME} (${CPACK_DEBIAN_PACKAGE_VERSION}) ${DISTRO_CODENAME}\; urgency=high")
set(CHANGELOG "  * Development Release.")
# two spaces between maintainer and timestamp is NOT a mistake
set(CHANGELOG_FOOTER " -- ${PACKAGE_CONTACT}  ${RFC2822_TIMESTAMP}")

file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/changelog ${CHANGELOG_HEADER}\n\n${CHANGELOG}\n\n${CHANGELOG_FOOTER})

add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/changelog.gz"
        COMMAND gzip -cn9 "${CMAKE_CURRENT_SOURCE_DIR}/changelog" > "${CMAKE_CURRENT_BINARY_DIR}/changelog.gz"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/changelog"
        COMMENT "Compressing changelog"
)

# first we can indicate the documentation build as an option and set it to ON by default
option(BUILD_DOC "Build documentation" ON)

# check if Doxygen is installed
find_package(Doxygen)
if (CMAKE_BUILD_TYPE MATCHES "^[Dd]ocuments")
    # build the docs
    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        # set input and output files
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

        # request to configure the file
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        message("Doxygen build started")

        # note the option ALL which allows to build the docs together with the application
        add_custom_target(Docs ALL
                COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                COMMENT "Generating API documentation with Doxygen"
                VERBATIM)

    else (DOXYGEN_FOUND)
        message("Doxygen needs to be installed to generate the doxygen documentation")
    endif (DOXYGEN_FOUND)
endif ()

add_executable(LocalTime main.cpp)
target_link_libraries(LocalTime cpp_local_time)

add_custom_target(changelog ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/changelog.gz")
add_dependencies(changelog cpp_local_time)

add_compile_definitions("NETBSD_INSPIRED=1")

list(APPEND LOCAL_TIME_SOURCE src/LocalTime.h src/LocalTime.cpp tzfiles/localtime.c)
add_library(cpp_local_time STATIC ${LOCAL_TIME_SOURCE})

install(
        TARGETS cpp_local_time
        EXPORT LocalTimeTargets
        COMPONENT LOCAL_TIME EXCLUDE_FROM_ALL
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )

install(
        DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/"
        EXPORT LocalTimeTargets
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}"
        COMPONENT LOCAL_TIME
        FILES_MATCHING PATTERN "*.h")

set(PACKAGE_INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}")
set(PACKAGE_LIBRARY_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
set(PACKAGE_LIBRARY_INSTALL_FILE "lib${PROJECT_NAME}.a")
set(PACKAGE_LIBRARY_NAME "${PROJECT_NAME}")

install(
        FILES "${CMAKE_CURRENT_BINARY_DIR}/changelog.gz"
        DESTINATION "${CMAKE_INSTALL_DOCDIR}"
        COMPONENT LOCAL_TIME
        )

install(
        EXPORT LocalTimeTargets
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LocalTime
        NAMESPACE LocalTime::  FILE LocalTimeTargets.cmake # Not sure if this is still needed
        COMPONENT LOCAL_TIME
)

include(CMakePackageConfigHelpers)

configure_package_config_file(
        "LocalTimeConfig.cmake.in"
        "LocalTimeConfig.cmake"
        INSTALL_DESTINATION ${LIB_INSTALL_LIBDIR}/cmake/LocalTime/
        PATH_VARS
            CMAKE_INSTALL_LIBDIR
        )
write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/LocalTimeConfigVersion.cmake
        COMPATIBILITY SameMajorVersion
)

install(
        FILES ${CMAKE_CURRENT_BINARY_DIR}/LocalTimeConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/LocalTimeConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LocalTime
        COMPONENT LOCAL_TIME)

set(CPACK_DEB_COMPONENT_INSTALL 1)
set(CPACK_DEBIAN_ENABLE_COMPONENT_DEPENDS 1)
set(CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT")
set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_RELEASE ${PROJECT_VERSION_TWEAK})
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_DEBIAN_LOCAL_TIME_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_PACKAGE_URL "https://github.com/pa28/cpp_local_time")
set(CPACK_RPM_PACKAGE_SUMMARY "Arbitrary timezone localtime.")
set(CPACK_RPM_PACKAGE_DESCRIPTION "Arbitrary timezone localtime.")
set(CPACK_RPM_PACKAGE_VENDOR "pa28.github.io")
set(CPACK_PACKAGE_CONTACT ${PACKAGE_CONTACT})
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION
        /usr/local
        /usr/local/bin
        /usr/local/share
        /usr/local/include
        /usr/lib/systemd
        /usr/lib/systemd/system
        )
include(CPack)

cpack_add_component(LOCAL_TIME
        DISPLAY_NAME "cpp_local_time"
        DESCRIPTION "Arbitrary timezone localtime."
        REQUIRED
        )
