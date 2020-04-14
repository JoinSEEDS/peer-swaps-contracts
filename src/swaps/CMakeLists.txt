cmake_minimum_required(VERSION 3.13)
project(seeds-swap)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)

add_contract( swaps swaps swaps.cpp)
target_include_directories( swaps PUBLIC ${CMAKE_SOURCE_DIR}/../include )
