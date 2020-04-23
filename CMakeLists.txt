cmake_minimum_required( VERSION 3.2 )
add_compile_options("-Wall" "-Werror")
set( CMAKE_CXX_STANDARD 17 )

add_executable(ThreadPoolExample src/main.cpp)
target_link_libraries(ThreadPoolExample rt pthread)
target_include_directories(ThreadPoolExample PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/include")

install(TARGETS
   ThreadPoolExample

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)