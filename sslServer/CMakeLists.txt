add_executable(SSLClient SSLClient.cpp)
target_include_directories(SSLClient PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(SSLClient ${Boost_LIBRARIES} boost_thread pthread)


find_package(OpenSSL 1.1.0 REQUIRED)
target_include_directories(SSLClient PRIVATE ${OPENSSL_INCLUDE_DIR})
target_link_libraries(SSLClient ${OPENSSL_LIBRARIES})


# add_executable(SSLServer SSLServer.cpp)
# target_include_directories(SSLServer PRIVATE ${Boost_INCLUDE_DIR})
# target_link_libraries(SSLServer ${Boost_LIBRARIES} boost_thread pthread)