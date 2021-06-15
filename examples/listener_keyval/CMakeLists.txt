cmake_minimum_required( VERSION 3.5 )

project(keyval)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../nanopb/extra)

find_package(eosio.cdt)
find_package(Nanopb REQUIRED)

include_directories(${NANOPB_INCLUDE_DIRS})

set(NANOPB_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/keyval.options)
NANOPB_GENERATE_CPP(PROTO_SRCS PROTO_HDRS keyval.proto)

message ("PROTO_SRCS = ${PROTO_SRCS}")
message ("PROTO_HDRS = ${PROTO_HDRS}")

include_directories(${CMAKE_CURRENT_BINARY_DIR})
set_source_files_properties(${PROTO_SRCS} ${PROTO_HDRS} PROPERTIES GENERATED TRUE)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -g -O0")

add_contract(keyval keyval 
keyval.cpp
keyval.pb.c
${CMAKE_CURRENT_SOURCE_DIR}/../../nanopb/pb_common.c
${CMAKE_CURRENT_SOURCE_DIR}/../../nanopb/pb_decode.c
)

target_include_directories(keyval
   PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
   PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../nanopb)
