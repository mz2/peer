# Install script for directory: /Users/mz2/PearComp/peer-backup/External/yaml-cpp

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/mz2/PearComp/peer-backup/External/yaml-cpp/build/libyaml-cpp.a")
  IF(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libyaml-cpp.a" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libyaml-cpp.a")
    EXECUTE_PROCESS(COMMAND "/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libyaml-cpp.a")
  ENDIF()
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/yaml-cpp" TYPE FILE FILES
    "/Users/mz2/PearComp/peer-backup/External/yaml-cpp/include/conversion.h"
    "/Users/mz2/PearComp/peer-backup/External/yaml-cpp/include/emitter.h"
    "/Users/mz2/PearComp/peer-backup/External/yaml-cpp/include/emittermanip.h"
    "/Users/mz2/PearComp/peer-backup/External/yaml-cpp/include/exceptions.h"
    "/Users/mz2/PearComp/peer-backup/External/yaml-cpp/include/iterator.h"
    "/Users/mz2/PearComp/peer-backup/External/yaml-cpp/include/mark.h"
    "/Users/mz2/PearComp/peer-backup/External/yaml-cpp/include/node.h"
    "/Users/mz2/PearComp/peer-backup/External/yaml-cpp/include/nodeimpl.h"
    "/Users/mz2/PearComp/peer-backup/External/yaml-cpp/include/nodereadimpl.h"
    "/Users/mz2/PearComp/peer-backup/External/yaml-cpp/include/nodeutil.h"
    "/Users/mz2/PearComp/peer-backup/External/yaml-cpp/include/noncopyable.h"
    "/Users/mz2/PearComp/peer-backup/External/yaml-cpp/include/null.h"
    "/Users/mz2/PearComp/peer-backup/External/yaml-cpp/include/ostream.h"
    "/Users/mz2/PearComp/peer-backup/External/yaml-cpp/include/parser.h"
    "/Users/mz2/PearComp/peer-backup/External/yaml-cpp/include/stlemitter.h"
    "/Users/mz2/PearComp/peer-backup/External/yaml-cpp/include/stlnode.h"
    "/Users/mz2/PearComp/peer-backup/External/yaml-cpp/include/traits.h"
    "/Users/mz2/PearComp/peer-backup/External/yaml-cpp/include/yaml.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/Users/mz2/PearComp/peer-backup/External/yaml-cpp/build/yaml-cpp.pc")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/Users/mz2/PearComp/peer-backup/External/yaml-cpp/build/test/cmake_install.cmake")
  INCLUDE("/Users/mz2/PearComp/peer-backup/External/yaml-cpp/build/util/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

IF(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
ELSE(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
ENDIF(CMAKE_INSTALL_COMPONENT)

FILE(WRITE "/Users/mz2/PearComp/peer-backup/External/yaml-cpp/build/${CMAKE_INSTALL_MANIFEST}" "")
FOREACH(file ${CMAKE_INSTALL_MANIFEST_FILES})
  FILE(APPEND "/Users/mz2/PearComp/peer-backup/External/yaml-cpp/build/${CMAKE_INSTALL_MANIFEST}" "${file}\n")
ENDFOREACH(file)
