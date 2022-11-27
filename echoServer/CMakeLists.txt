add_executable(synClient synClient.cpp)
target_include_directories(synClient PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(synClient ${Boost_LIBRARIES} boost_thread pthread)

add_executable(synServer synServer.cpp)
target_include_directories(synServer PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(synServer ${Boost_LIBRARIES} boost_thread pthread)

add_executable(asynServer asynServer.cpp)
target_include_directories(asynServer PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(asynServer ${Boost_LIBRARIES} boost_thread pthread)
