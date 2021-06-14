cmake_minimum_required( VERSION 3.5 )

project(pbtx)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../nanopb/extra)

find_package(eosio.cdt)
find_package(Nanopb REQUIRED)

include_directories(${NANOPB_INCLUDE_DIRS})

NANOPB_GENERATE_CPP(PROTO_SRCS PROTO_HDRS RELPATH ./../ ./../pbtx.proto)

message ("PROTO_SRCS = ${PROTO_SRCS}")
message ("PROTO_HDRS = ${PROTO_HDRS}")

include_directories(${CMAKE_CURRENT_BINARY_DIR})
set_source_files_properties(${PROTO_SRCS} ${PROTO_HDRS} PROPERTIES GENERATED TRUE)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -g -O0")

add_contract(pbtx pbtx 
pbtx.cpp
pbtx.pb.c
${CMAKE_CURRENT_SOURCE_DIR}/../nanopb/pb_common.c
${CMAKE_CURRENT_SOURCE_DIR}/../nanopb/pb_decode.c
)

target_include_directories(pbtx
   PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
   PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../nanopb)

set_target_properties(pbtx
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ricardian/pbtx.contracts.md.in ${CMAKE_CURRENT_BINARY_DIR}/ricardian/pbtx.contracts.md @ONLY )

target_compile_options( pbtx PUBLIC -R${CMAKE_CURRENT_SOURCE_DIR}/pbtx -R${CMAKE_CURRENT_BINARY_DIR}/ricardian )
