#pragma once

#include <eosio/symbol.hpp>

using namespace eosio;
using std::string;

namespace common {

    static const symbol         S_USD                           ("USD", 2);

  //  static const name           EOS_CONTRACT                    ("eosio.token"_n);
    static const symbol         S_EOS                           ("EOS", 4);

    static const uint64_t       SCALER                          = 100000000;

    static const uint8_t        PAUSED                          = 1;
    static const uint8_t        UNPAUSED                        = 0;

    static const string         OPEN                            = "OPEN";
    static const string         ESCROW                          = "ESCROW";
    static const string         FULFILLED                       = "FULFILLED";
    static const string         DISPUTE                         = "DISPUTE";
    static const string         BUYER_APPROVED                  = "BUYER_APPROVED";


    static const uint8_t        BUY                             = 1;
    static const uint8_t        SELL                            = 2;
    static const string         EXACT_PRICE                     = "EXACT_PRICE";
    static const string         MARKET_VARIANCE                 = "MARKET_VARIANCE";
    
    static const asset          ONE_EOS                         = asset { 10000, S_EOS };
    static const asset          ONE_USD                         = asset { 100, S_USD };

}