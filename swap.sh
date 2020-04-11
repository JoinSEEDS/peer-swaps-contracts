Private key: 5Hrs8eqUVS7LjVXUjLEr4To8MafhQqQji4VPbWLnKAZLwZD3ZTc
Public key: EOS6SiERamZCNTnVV8xbH48y8vGa4YrrLESyCjGLyFEQJc8QNrDR8

cleos create account eosio swaps EOS6SiERamZCNTnVV8xbH48y8vGa4YrrLESyCjGLyFEQJc8QNrDR8 EOS6SiERamZCNTnVV8xbH48y8vGa4YrrLESyCjGLyFEQJc8QNrDR8
cleos create account eosio seller EOS6SiERamZCNTnVV8xbH48y8vGa4YrrLESyCjGLyFEQJc8QNrDR8 EOS6SiERamZCNTnVV8xbH48y8vGa4YrrLESyCjGLyFEQJc8QNrDR8
cleos create account eosio buyer EOS6SiERamZCNTnVV8xbH48y8vGa4YrrLESyCjGLyFEQJc8QNrDR8 EOS6SiERamZCNTnVV8xbH48y8vGa4YrrLESyCjGLyFEQJc8QNrDR8
cleos create account eosio revenue EOS6SiERamZCNTnVV8xbH48y8vGa4YrrLESyCjGLyFEQJc8QNrDR8 EOS6SiERamZCNTnVV8xbH48y8vGa4YrrLESyCjGLyFEQJc8QNrDR8
cleos create account eosio escrow EOS6SiERamZCNTnVV8xbH48y8vGa4YrrLESyCjGLyFEQJc8QNrDR8 EOS6SiERamZCNTnVV8xbH48y8vGa4YrrLESyCjGLyFEQJc8QNrDR8
cleos create account eosio mockeostoken EOS6SiERamZCNTnVV8xbH48y8vGa4YrrLESyCjGLyFEQJc8QNrDR8 EOS6SiERamZCNTnVV8xbH48y8vGa4YrrLESyCjGLyFEQJc8QNrDR8

source perm.json

cleos set contract swaps swaps/swaps
cleos set contract mockeostoken swaps/eosiotoken

cleos push action mockeostoken create '["mockeostoken", "1000000000.0000 MOCKEOS"]' -p mockeostoken
cleos push action mockeostoken issue '["seller", "100000.0000 MOCKEOS", "memo"]' -p mockeostoken

cleos push action swaps setconfig '["escrow", "revenue", 0.0150000000000, "3.50 USD", 0.01500000000, \
    "0.5000 MOCKEOS", "5.0000 MOCKEOS", "10000.0000 MOCKEOS", "1000.0000 MOCKEOS", "mockeostoken"]' -p swaps

cleos push action mockeostoken transfer '["seller", "swaps", "10.0000 MOCKEOS", "memo"]' -p seller

cleos push action swaps create '{"account":"seller", "order_type":2, "amount":"10.0000 MOCKEOS", \
    "min_trx":"10.0000 MOCKEOS", "price_type":"EXACT_PRICE", "price_per_eos":"5.00 USD", \
    "price_var":0.000000000, "payment_methods": ["VENMO", "PAYPAL"]}' -p seller

cleos push action swaps create '{"account":"seller", "order_type":2, "amount":"1.0000 MOCKEOS", \
    "price_type":"MARKET_VARIANCE", "price_per_eos":"0.00 USD", "price_var":0.10000000, \
    "payment_methods": ["VENMO", "PAYPAL"], "allow_partial":0}' -p seller

cleos get table swaps swaps orders


cleos push action swaps accept '[937834528, "buyer", "VENMO", "10.0000 MOCKEOS"]' -p buyer

cleos push action swaps approvepay '[3370420420, "buyer"]' -p buyer
cleos push action swaps approvepay '[770137015, "seller"]' -p seller