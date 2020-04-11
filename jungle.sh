Private key: 5K6z3PaivZXuMjEutFkFMCXgMBABf1kQGsyAvomaMVEsrQs8BR5
Public key: EOS5ESMDwUyAiZ7zSYHRiWF6r9k9R2SE8nf5Zz6YDhxtBA55Bihpz

swapsswaps11
swapsswaps12
swapsswaps13
swapsrevenue
swapsescrow1
swapsseller1
swapsbuyer11
swapsreferr1

swapsswaps14
swapstrail11
swaps

source perm.json

cleos -u https://jungle2.cryptolions.io set contract swapsswaps13 swaps/swaps
cleos -u https://jungle2.cryptolions.io set contract swapsrevenue swaps/revenue

cleos -u https://jungle2.cryptolions.io push action swapsswaps13 setconfig '["swapsescrow1", "swapsrevenue", 0.0050000000000, \
    "3.50 USD", 0.010000000000, "1.5000 EOS", "2.5000 EOS", "10000.0000 EOS", "1000.0000 EOS", "eosio.token"]' -p swapsswaps13

cleos -u https://jungle2.cryptolions.io push action swaps setconfig '["escrow", "revenue", 0.0150000000000, 
    "3.50 USD", 0.01500000000, "0.5000 MOCKEOS", "5.0000 MOCKEOS", "10000.0000 MOCKEOS", "1000.0000 MOCKEOS", "eosio"]' -p swaps

cleos -u https://jungle2.cryptolions.io push action eosio.token transfer '["swapsseller1", "swapsswaps13", "10.0000 EOS", "memo"]' -p swapsseller1
cleos -u https://jungle2.cryptolions.io push action swapsswaps13 create '{"account":"swapsseller1", "order_type":2, "amount":"10.0000 EOS", "min_trx":"3.0000 EOS", "price_type":"EXACT_PRICE", "price_per_eos":"5.00 USD", "price_var":0.000000000, "payment_methods": ["VENMO", "PAYPAL"], "app_account":"swapsreferr1"}' -p swapsseller1
#cleos -u https://jungle2.cryptolions.io push action swapsswaps13 create '{"account":"swapsseller1", "order_type":2, "amount":"1.0000 EOS", "price_type":"MARKET_VARIANCE", "price_per_eos":"0.00 USD", "price_var":0.10000000, "payment_methods": ["VENMO", "PAYPAL"], "allow_partial":0}' -p swapsseller1

cleos -u https://jungle2.cryptolions.io get table swapsswaps13 swapsswaps13 orders

cleos -u https://jungle2.cryptolions.io push action swapsswaps13 accept '[3774398541, "swapsbuyer11", "VENMO", "10.0000 EOS"]' -p swapsbuyer11

cleos -u https://jungle2.cryptolions.io push action swapsswaps13 approvepay '[3774398541, "swapsseller1"]' -p swapsseller1


cleos -u https://jungle2.cryptolions.io push action swapsrevenue addprovider '["swapsswaps13", "testing provider"]' -p swapsrevenue
cleos -u https://jungle2.cryptolions.io push action swapsrevenue addreceiver '["swapsreferr1", "testing receiver"]' -p swapsrevenue
cleos -u https://jungle2.cryptolions.io push action swapsrevenue addtrustline '["swapsswaps13", "swapsreferr1", 0.2500000000, "swaps12 trustline to referrer1"]' -p swapsrevenue

cleos -u https://jungle2.cryptolions.io push action swapsrevenue paypartner '["swapsreferr1", "0.0000 EOS"]' -p swapsrevenue

cleos -u https://jungle2.cryptolions.io push action eosio updateauth '{
    "account": "swapsswaps13",
    "permission": "active",
    "parent": "owner",
    "auth": {
        "keys": [
            {
                "key": "EOS6SiERamZCNTnVV8xbH48y8vGa4YrrLESyCjGLyFEQJc8QNrDR8",
                "weight": 1
            }
        ],
        "threshold": 1,
        "accounts": [
            {
                "permission": {
                    "actor": "swapsswaps13",
                    "permission": "eosio.code"
                },
                "weight": 1
            }
        ],
        "waits": []
    }
}' -p swapsswaps13@owner

cleos -u https://jungle2.cryptolions.io push action eosio updateauth '{
    "account": "swapsswaps12",
    "permission": "owner",
    "parent": "",
    "auth": {
        "keys": [
            {
                "key": "EOS6SiERamZCNTnVV8xbH48y8vGa4YrrLESyCjGLyFEQJc8QNrDR8",
                "weight": 1
            }
        ],
        "threshold": 1,
        "accounts": [
            {
                "permission": {
                    "actor": "swapsswaps12",
                    "permission": "eosio.code"
                },
                "weight": 1
            }
        ],
        "waits": []
    }
}' -p swapsswaps12@owner







biggertoken1
biggertrail1
biggerdapp11
biggerprime1
biggeruser11
biggeruser12
biggeruser13

cleos -u https://jungle2.cryptolions.io set contract biggertoken1 swaps/token
cleos -u https://jungle2.cryptolions.io set contract biggertrail1 swaps/trail
cleos -u https://jungle2.cryptolions.io set contract biggerdapp11 swaps/swaps
cleos -u https://jungle2.cryptolions.io set contract biggerprime1 ~/dev/direct-democracy/directdemocr/directdemocr

cleos -u https://jungle2.cryptolions.io push action biggertoken1 create '["biggertoken1", "1000000000.0000 BIG"]' -p biggertoken1
cleos -u https://jungle2.cryptolions.io push action biggertoken1 issue '["biggertoken1", "1000000000.0000 BIG", "memo"]' -p biggertoken1

cleos -u https://jungle2.cryptolions.io push action biggertoken1 transfer '["biggertoken1", "biggeruser11", "1000.0000 BIG", "memo"]' -p biggertoken1
cleos -u https://jungle2.cryptolions.io push action biggertoken1 transfer '["biggertoken1", "biggeruser12", "750.0000 BIG", "memo"]' -p biggertoken1
cleos -u https://jungle2.cryptolions.io push action biggertoken1 transfer '["biggertoken1", "biggeruser13", "300.0000 BIG", "memo"]' -p biggertoken1
cleos -u https://jungle2.cryptolions.io push action biggertoken1 transfer '["biggertoken1", "biggerprime1", "500.0000 BIG", "deposit"]' -p biggertoken1
cleos -u https://jungle2.cryptolions.io push action biggertoken1 transfer '["biggerprime1", "biggertrail1", "500.0000 BIG", "deposit"]' -p biggerprime1

cleos -u https://jungle2.cryptolions.io push action biggertrail1 setconfig '["2.0", true]' -p biggertrail1
cleos -u https://jungle2.cryptolions.io push action biggerprime1 setconfig '["biggertrail1", "1000000000.0000 VOTE"]' -p biggerprime1

cleos -u https://jungle2.cryptolions.io push action biggertrail1 newregistry '["biggerprime1","1000000000.0000 VOTE", "public"]' -p biggerprime1

cleos -u https://jungle2.cryptolions.io push action biggertrail1 regvoter '["biggeruser11", "4,VOTE", ""]' -p biggeruser11
cleos -u https://jungle2.cryptolions.io push action biggertrail1 regvoter '["biggeruser12", "4,VOTE", ""]' -p biggeruser12
cleos -u https://jungle2.cryptolions.io push action biggertrail1 regvoter '["biggeruser13", "4,VOTE", ""]' -p biggeruser13
cleos -u https://jungle2.cryptolions.io push action biggertrail1 regvoter '["biggerprime1", "4,VOTE", ""]' -p biggerprime1

cleos -u https://jungle2.cryptolions.io push action biggertrail1 mint '["biggeruser11", "1000.0000 VOTE", "original mint"]' -p biggerprime1
cleos -u https://jungle2.cryptolions.io push action biggertrail1 mint '["biggeruser12", "750.0000 VOTE", "original mint"]' -p biggerprime1
cleos -u https://jungle2.cryptolions.io push action biggertrail1 mint '["biggeruser13", "300.0000 VOTE", "original mint"]' -p biggerprime1

# cleos -u https://jungle2.cryptolions.io push action biggertrail1 mint '["biggerprime1", "1000.0000 VOTE", "original mint"]' -p biggerprime1

cleos -u https://jungle2.cryptolions.io push action biggerdapp11 printversion '[]' -p biggeruser11 
# change the Swap version to 2.0

cleos -u https://jungle2.cryptolions.io push action -sjd -x 86400 biggerprime1 addkey '["biggerdapp11", "EOS5ESMDwUyAiZ7zSYHRiWF6r9k9R2SE8nf5Zz6YDhxtBA55Bihpz"]' -p biggerprime1 > updperm.json
cleos -u https://jungle2.cryptolions.io push action -sjd -x 86400 biggerprime1 permtoprime '["biggerdapp11"]' -p biggerprime1 > updperm.json

cleos -u https://jungle2.cryptolions.io push action -sjd -x 86400 biggerdapp11 updperm '[]' -p biggerdapp11 > updperm.json
cleos -u https://jungle2.cryptolions.io set contract -sjd -x 186400 biggerdapp11 swaps/swaps > upgrade.json


# Update upgrade.json file
cleos -u https://jungle2.cryptolions.io push action biggerprime1 propose ./upgrade.json -p biggeruser11
cleos -u https://jungle2.cryptolions.io push action biggerprime1 propose ./updperm.json -p biggeruser11

cleos -u https://jungle2.cryptolions.io push action biggerprime1 remproposal '["upgrade2"]' -p biggeruser11

# send tokens
cleos -u https://jungle2.cryptolions.io push action -sjd -x 86400 eosio.token transfer '["biggerdapp11", "biggerprime1", "26307.0000 EOS", "memo"]' -p biggerdapp11 > transfer.json
cleos -u https://jungle2.cryptolions.io push action biggerprime1 propose ./transfer.json -p biggeruser11
cleos -u https://jungle2.cryptolions.io push action biggertrail1 castvote '["biggeruser11", "transfer", ["pass"]]' -p biggeruser11
cleos -u https://jungle2.cryptolions.io push action biggertrail1 castvote '["biggeruser12", "transfer", ["pass"]]' -p biggeruser12
cleos -u https://jungle2.cryptolions.io push action biggertrail1 castvote '["biggeruser13", "transfer", ["fail"]]' -p biggeruser13
cleos -u https://jungle2.cryptolions.io push action biggerprime1 exec '["biggeruser11", "transfer", "biggeruser11"]' -p biggeruser11


cleos -u https://jungle2.cryptolions.io get table biggertrail1 biggertrail1 registries
cleos -u https://jungle2.cryptolions.io get table biggertrail1 biggertrail1 ballots
cleos -u https://jungle2.cryptolions.io get table biggertrail1 biggertrail1 voters

cleos -u https://jungle2.cryptolions.io push action biggertrail1 castvote '["biggeruser11", "permtoprime", ["pass"]]' -p biggeruser11
cleos -u https://jungle2.cryptolions.io push action biggertrail1 castvote '["biggeruser12", "permtoprime", ["pass"]]' -p biggeruser12
cleos -u https://jungle2.cryptolions.io push action biggertrail1 castvote '["biggeruser13", "permtoprime", ["fail"]]' -p biggeruser13

cleos -u https://jungle2.cryptolions.io push action biggerprime1 exec '["biggeruser11", "permtoprime", "biggeruser11"]' -p biggeruser11


cleos -u https://jungle2.cryptolions.io push action biggertrail1 castvote '["biggeruser11", "updperm3", ["pass"]]' -p biggeruser11
cleos -u https://jungle2.cryptolions.io push action biggertrail1 castvote '["biggeruser12", "updperm3", ["pass"]]' -p biggeruser12
cleos -u https://jungle2.cryptolions.io push action biggertrail1 castvote '["biggeruser13", "updperm3", ["fail"]]' -p biggeruser13
cleos -u https://jungle2.cryptolions.io push action biggerprime1 exec '["biggeruser11", "updperm3", "biggeruser11"]' -p biggeruser11



cleos -u https://jungle2.cryptolions.io push action biggerdapp11 printversion '[]' -p biggeruser11


cleos -u https://jungle2.cryptolions.io get table biggerprime1 biggerprime1 proposals



cleos -u https://jungle2.cryptolions.io push action biggertrail1 deleteballot '["upgrade"]' -p biggerprime1
cleos -u https://jungle2.cryptolions.io push action biggertrail1 closeballot '["upgrade", false]' -p biggerprime1
cleos -u https://jungle2.cryptolions.io push action biggertrail1 cancelballot


cleos -u https://jungle2.cryptolions.io push action eosio updateauth '{
    "account": "biggerprime1",
    "permission": "active",
    "parent": "owner",
    "auth": {
        "keys": [
            {
                "key": "EOS5ESMDwUyAiZ7zSYHRiWF6r9k9R2SE8nf5Zz6YDhxtBA55Bihpz",
                "weight": 1
            }
        ],
        "threshold": 1,
        "accounts": [
            {
                "permission": {
                    "actor": "biggerprime1",
                    "permission": "eosio.code"
                },
                "weight": 1
            }
        ],
        "waits": []
    }
}' -p biggerprime1@owner


cleos -u https://jungle2.cryptolions.io push action eosio updateauth '{
    "account": "biggerdapp11",
    "permission": "owner",
    "parent": "",
    "auth": {
        "keys": [
           
        ],
        "threshold": 1,
        "accounts": [
            {
                "permission": {
                    "actor": "biggerprime1",
                    "permission": "eosio.code"
                },
                "weight": 1
            }
        ],
        "waits": []
    }
}' -p biggerdapp11@owner