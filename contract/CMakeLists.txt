cmake_minimum_required( VERSION 3.5 )

project(pbtx)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")

find_package(eosio.cdt)
find_package(Nanopb REQUIRED)


add_contract(pbtx pbtx 
pbtx.cpp pbtx.pb.c pb_common.c pb_decode.c
)

target_include_directories(pbtx
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${NANOPB_INCLUDE_DIRS})

set_target_properties(pbtx
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ricardian/pbtx.contracts.md.in ${CMAKE_CURRENT_BINARY_DIR}/ricardian/pbtx.contracts.md @ONLY )

target_compile_options( pbtx PUBLIC -R${CMAKE_CURRENT_SOURCE_DIR}/pbtx -R${CMAKE_CURRENT_BINARY_DIR}/ricardian )
