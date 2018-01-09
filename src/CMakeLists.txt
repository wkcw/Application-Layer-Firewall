unset(CMAKE_OSX_DEPLOYMENT_TARGET)

cmake_minimum_required(VERSION 2.8)

PROJECT(netfilter_sjtu)

#CMAKE CONFIGURATION
set(CMAKE_USE_RELATIVE_PATHS ON)
cmake_policy(SET CMP0015 NEW)

# path
set(PROJECT_PATH ${CMAKE_SOURCE_DIR})
set(VENDOR_PATH ${PROJECT_PATH}/vendor)
set(VENDOR_INCLUDE_DIRECTORYS ${LIBEVENT_PATH}/include)

# compile flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")

## THIRD PARTY
#set(LIBEVENT_PATH ${VENDOR_PATH}/libevent)


include_directories(
        ${PROJECT_PATH}
        ${VENDOR_PATH}
)

link_directories(
        ${LIBEVENT_PATH}/.libs
        /usr/local/lib
)

set(DEPENDENCY_FILES
        filter/PackerFilterService.cpp
        common/ConfigAdapter.cpp
        util/Singleton.cpp
        util/StorageAdapter.cpp
        rule/Rule.cpp
        rule/RuleCollection.cpp
        server/MessageHandler.cpp
        util/LockInterface.cpp
        filter/NetFilterServiceProvider.cpp
        rule/PacketExtractor.cpp
        util/BufferProvider.cpp
        rule/Judge.cpp
        net/BaseHandler.cpp
		rule/RuleManager.cpp
		util/ThreadWrapper.cpp
		monitor/LogService.cpp)

add_executable(filter_server
        server/server.cpp
		${DEPENDENCY_FILES})

target_link_libraries(
        filter_server
        pthread
        mnl
        netfilter_queue
        nfnetlink
)

