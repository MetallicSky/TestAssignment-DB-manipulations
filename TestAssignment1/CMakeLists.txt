# CMakeList.txt : CMake project for TestAssignment1, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (TestAssignment1 "TestAssignment1.cpp" "TestAssignment1.h" "WorkerDB.h" "WorkerDB.cpp")

# SQLite
set(SQLITE3_SRC "${CMAKE_SOURCE_DIR}/external/sqlite/sqlite3.c")
set(SQLITE3_HEADER "${CMAKE_SOURCE_DIR}/external/sqlite/sqlite3.h")
add_library(sqlite3 STATIC ${SQLITE3_SRC} "WorkerDB.h" "WorkerDB.cpp")
target_include_directories(sqlite3 PUBLIC ${CMAKE_SOURCE_DIR}/external/sqlite)
target_link_libraries(TestAssignment1 PRIVATE sqlite3)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET TestAssignment1 PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
