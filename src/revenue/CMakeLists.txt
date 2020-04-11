cmake_minimum_required(VERSION 3.13)
project(revenue)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)

add_contract( revenue revenue revenue.cpp )
target_include_directories( revenue PUBLIC ${CMAKE_SOURCE_DIR}/../include )
