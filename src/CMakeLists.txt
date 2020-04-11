cmake_minimum_required(VERSION 3.13)
project(swaps)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)

add_subdirectory(swaps)
add_subdirectory(revenue)
