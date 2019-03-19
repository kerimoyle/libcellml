# Copyright libCellML Contributors
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.cmake_minimum_required (VERSION 3.1)

find_package(Python ${PREFERRED_PYTHON_VERSION} COMPONENTS Interpreter Development)

find_program(CLANG_TIDY_EXE NAMES ${PREFERRED_CLANG_TIDY_NAMES} clang-tidy)
find_program(FIND_EXE NAMES ${PREFERRED_FIND_NAMES} find)
find_program(GCOV_EXE NAMES ${PREFERRED_GCOV_NAMES} gcov)
find_program(VALGRIND_EXE NAMES ${PREFERRED_VALGRIND_NAMES} valgrind)

find_package(Doxygen)
find_package(Sphinx)
find_package(SWIG 3)

set(HAVE_LIBXML2_CONFIG FALSE)
if (MSVC)
  # If we want to use config packages on Windows with Visual Studio,
  # we need to have two find_package calls and explicitly state that
  # we wish to use Config mode in the first call.  Finding LibXml2 in config mode
  # is the preferred method so we will try this first quietly.
  #
  # This does change how we get information about include paths and such so we
  # need to track how we found LibXml2.
  find_package(LibXml2 CONFIG QUIET)
  if (LibXml2_FOUND)
    set(HAVE_LIBXML2_CONFIG TRUE)
  else ()
    find_package(LibXml2 REQUIRED)
  endif ()
else ()
  find_package(LibXml2 REQUIRED)
endif ()

if (CLANG_TIDY_EXE)
  set(CLANG_TIDY_AVAILABLE TRUE CACHE BOOL "Executable required to perform static analysis is available.")
endif ()

if(FIND_EXE AND GCOV_EXE AND Python_Interpreter_FOUND)
  set(COVERAGE_TESTING_AVAILABLE TRUE CACHE BOOL "Executables required to run the coverage testing are available.")
endif()

if (SWIG_EXECUTABLE)
  set(BINDINGS_AVAILABLE TRUE CACHE BOOL "Executable required to generate bindings is available.")
endif ()

if(VALGRIND_EXE AND Python_Interpreter_FOUND)
  set(VALGRIND_TESTING_AVAILABLE TRUE CACHE BOOL "Executable required to run valgrind testing is available.")
endif()

mark_as_advanced(
  CLANG_TIDY_EXE CLANG_TIDY_AVAILABLE
  FIND_EXE GCOV_EXE COVERAGE_TESTING_AVAILABLE
  SWIG_EXECUTABLE BINDINGS_AVAILABLE
  VALGRIND_EXE VALGRIND_TESTING_AVAILABLE
)

