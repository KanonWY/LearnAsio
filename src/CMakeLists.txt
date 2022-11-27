
add_executable(asiolearn main.cpp)
target_link_libraries(asiolearn boost_thread)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(asiolearn ${CMAKE_THREAD_LIBS_INIT})



find_package(Boost 1.66.0 REQUIRED COMPONENTS system program_options)
target_include_directories(asiolearn PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(asiolearn ${Boost_LIBRARIES})


add_executable(bindTest bindTest.cpp)
target_include_directories(bindTest PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(bindTest ${Boost_LIBRARIES} boost_thread pthread)


add_executable(asiotest1 asio_test1.cpp)
target_include_directories(asiotest1 PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(asiotest1 ${Boost_LIBRARIES} boost_thread pthread)

add_executable(asioPost postTest.cpp)
target_include_directories(asioPost PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(asioPost ${Boost_LIBRARIES} boost_thread pthread)

add_executable(testexcept testexcept.cpp)
target_include_directories(testexcept PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(testexcept ${Boost_LIBRARIES} boost_thread pthread)


add_executable(testexpiringtimer expiringtimer.cpp)
target_include_directories(testexpiringtimer PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(testexpiringtimer ${Boost_LIBRARIES} boost_thread pthread)


add_executable(testexpiringtimer2 expiringtimer2.cpp)
target_include_directories(testexpiringtimer2 PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(testexpiringtimer2 ${Boost_LIBRARIES} boost_thread pthread)


add_executable(strandTimer strandTimer.cpp)
target_include_directories(strandTimer PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(strandTimer ${Boost_LIBRARIES} boost_thread pthread)


add_executable(synchronousclient synchronousclient.cpp)
target_include_directories(synchronousclient PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(synchronousclient ${Boost_LIBRARIES} boost_thread pthread)

# asyncServer.cpp

add_executable(asynclient asynclient.cpp)
target_include_directories(asynclient PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(asynclient ${Boost_LIBRARIES} boost_thread pthread)


add_executable(asyncServer asyncServer.cpp)
target_include_directories(asyncServer PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(asyncServer ${Boost_LIBRARIES} boost_thread pthread)


add_executable(read_write_socket read_write_socket.cpp)
target_include_directories(read_write_socket PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(read_write_socket ${Boost_LIBRARIES} boost_thread pthread)