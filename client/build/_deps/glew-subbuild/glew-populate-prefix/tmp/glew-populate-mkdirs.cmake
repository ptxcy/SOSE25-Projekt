# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/Users/pni/Uni/SOSE25-Projekt/client/build/_deps/glew-src")
  file(MAKE_DIRECTORY "/Users/pni/Uni/SOSE25-Projekt/client/build/_deps/glew-src")
endif()
file(MAKE_DIRECTORY
  "/Users/pni/Uni/SOSE25-Projekt/client/build/_deps/glew-build"
  "/Users/pni/Uni/SOSE25-Projekt/client/build/_deps/glew-subbuild/glew-populate-prefix"
  "/Users/pni/Uni/SOSE25-Projekt/client/build/_deps/glew-subbuild/glew-populate-prefix/tmp"
  "/Users/pni/Uni/SOSE25-Projekt/client/build/_deps/glew-subbuild/glew-populate-prefix/src/glew-populate-stamp"
  "/Users/pni/Uni/SOSE25-Projekt/client/build/_deps/glew-subbuild/glew-populate-prefix/src"
  "/Users/pni/Uni/SOSE25-Projekt/client/build/_deps/glew-subbuild/glew-populate-prefix/src/glew-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/pni/Uni/SOSE25-Projekt/client/build/_deps/glew-subbuild/glew-populate-prefix/src/glew-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/pni/Uni/SOSE25-Projekt/client/build/_deps/glew-subbuild/glew-populate-prefix/src/glew-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
