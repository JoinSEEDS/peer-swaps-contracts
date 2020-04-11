swapsfreedom
swapsrevenue
swapsescrow1
swapsreferr1
swapsseller1
swapsbuyer11

Private key: 5JHZTEgbB5mxXaUtfJB1jX2FtNxQ4r9GM31hfufnWWkr1dAa6nJ
Public key: EOS6yUdStjy8vDAQZkGeuYQgsvjnpULMrqZXRSa3XvS36c9cD546W

cleos -u https://telos.eosphere.io set contract swapsfreedom swaps/swaps
cleos -u https://telos.eosphere.io set contract swapsrevenue swaps/revenue


cleos -u https://telos.eosphere.io push action swapsfreedom setconfig '["swapsfreedom", "swapsrevenue", 0.0050000000000,"3.50 USD", 0.010000000000, "0.0100 TLOS", "0.0100 TLOS", "10000.0000 TLOS", "1000.0000 TLOS", "eosio.token"]' -p swapsfreedom

cleos -u https://telos.eosphere.io push action eosio.token transfer '["swapsseller1", "swapsfreedom", "0.5000 TLOS", "memo"]' -p swapsseller1
cleos -u https://telos.eosphere.io push action swapsfreedom create '{"account":"swapsseller1", "order_type":2, "amount":"0.0200 TLOS", "min_trx":"0.0200 TLOS", "price_type":"EXACT_PRICE", "price_per_eos":"5.00 USD", "price_var":0.000000000, "payment_methods": ["VENMO", "PAYPAL"], "app_account":"swapsreferr1"}' -p swapsseller1

cleos -u https://telos.eosphere.io get table swapsfreedom swapsfreedom orders

cleos -u https://telos.eosphere.io push action swapsrevenue addprovider '["swapsfreedom", "testing provider"]' -p swapsrevenue
cleos -u https://telos.eosphere.io push action swapsrevenue addreceiver '["swapsreferr1", "testing receiver"]' -p swapsrevenue
cleos -u https://telos.eosphere.io push action swapsrevenue addtrustline '["swapsfreedom", "swapsreferr1", 0.2500000000, "swaps trustline to referrer1"]' -p swapsrevenue


cleos -u https://telos.eosphere.io push action swapsfreedom accept '[4081525996, "swapsbuyer11", "VENMO", "0.0200 TLOS"]' -p swapsbuyer11
cleos -u https://telos.eosphere.io push action swapsfreedom approvepay '[4081525996, "swapsseller1"]' -p swapsseller1
cleos -u https://telos.eosphere.io push action swapsrevenue paypartner '["swapsreferr1", "0.0000 EOS"]' -p swapsrevenue


cleos -u https://telos.eosphere.io push action eosio updateauth '{
    "account": "swapsfreedom",
    "permission": "active",
    "parent": "owner",
    "auth": {
        "keys": [
            {
                "key": "EOS6yUdStjy8vDAQZkGeuYQgsvjnpULMrqZXRSa3XvS36c9cD546W",
                "weight": 1
            }
        ],
        "threshold": 1,
        "accounts": [
            {
                "permission": {
                    "actor": "swapsfreedom",
                    "permission": "eosio.code"
                },
                "weight": 1
            }
        ],
        "waits": []
    }
}' -p swapsfreedom@owner

cleos -u https://telos.eosphere.io push action eosio updateauth '{
    "account": "swapsrevenue",
    "permission": "owner",
    "parent": "",
    "auth": {
        "keys": [
            {
                "key": "EOS6yUdStjy8vDAQZkGeuYQgsvjnpULMrqZXRSa3XvS36c9cD546W",
                "weight": 1
            }
        ],
        "threshold": 1,
        "accounts": [
            {
                "permission": {
                    "actor": "swapsrevenue",
                    "permission": "eosio.code"
                },
                "weight": 1
            }
        ],
        "waits": []
    }
}' -p swapsrevenue@owner