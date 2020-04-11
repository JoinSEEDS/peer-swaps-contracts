**NOTE: Operators use at their own risk. Developers do not accept any risks associated with faulty code or regulatory concerns. Software is written for research purposes only. Not for use in production. Use only with tobacco. Do not try this at home. Disclaimer to cover all possible types of disclaimers.**

## Quick Start
```
git clone git@github.com:JoinSEEDS/seeds-swap-contracts.git
cd seeds-swaps-contracts
cmake .
make
```

## Swap Smart Contracts
### Platform configuration
The configuration is once per "swaps" smart contract, which handles a single trading pair but many orders. 

- escrow account: where seller's escrow funds are held
- revenue account: where the revenue goes when an order is closed. The revenue smart contract handles trustlines between revenue providers and revenue earners (commission-based lead generators)
- fees: all fees are taken from the seller (since that is what we have in escrow), but they each pay a fee that gets reflected in the amounts. There is a flat fee and flat fee per trade on each side.
- min/max: the platform has a minimum and maximum order size
- max open per user: this configuration places a maximum that one account can have in open orders, primarily for use during BETA. There is also a global throttle and user-based throttle to control for volume.

```
      ACTION setconfig (const name& escrow_account,
                        const name& revenue_account,
                        const float& usd_holder_perc_fee,
                        const asset& usd_holder_asset_fee,
                        const float& eos_holder_perc_fee,
                        const asset& eos_holder_asset_fee,
                        const asset& order_min,
                        const asset& order_max,
                        const asset& max_open_per_user,
                        const name& eos_token_contract);
```

### Order Cycle
#### Step 1: Create an Order
Create action creates an order with the following action. Orders can be one to many, meaning that someone can post a sell order for 100 and it can provide to multiple buy orders.  There is a minimum transaction size that can be specified for order placement.

Price type can be an Exact price (in the trading pair) or a % Variance to a market price provided by a trusted Oracle.

If this order is a sell order, the balance of the token must be available in the escrow contract (usually the same contract holding this contract).

```
      ACTION create (const name& account, 
                     const uint8_t& order_type, 
                     const asset& amount, 
                     const asset& min_trx,
                     const string& price_type, 
                     const asset& price_per_eos,
                     const float& price_var, 
                     const vector<string> payment_methods,
                     const name& app_account);
```

- Order type is either a buy order or sell order
- min_trx is the minimum transaction size for this order 
- price_per_eos is used with exact pricing
- price_var is used with variance pricing
- the order creator provides a list of payment methods, such as VENMO, PAYPAL, ARTWORK, BANANAS, BTC, whatever


```
cleos push action swaps create '{"account":"seller", "order_type":2, "amount":"10.0000 MOCKEOS", \
    "min_trx":"10.0000 MOCKEOS", "price_type":"EXACT_PRICE", "price_per_eos":"5.00 USD", \
    "price_var":0.000000000, "payment_methods": ["VENMO", "PAYPAL"]}' -p seller
```

#### Step 2: Accept Order
Another user may accept the order by calling this action. It includes the order ID, their account name, payment method, and the amount.  

If the order acceptance is less than the existing order, then a partial order is created and deducted from the existing order.  In addition, the payment_method selected must be in the payment_methods selected by the original order creator.

```
      ACTION accept (const uint64_t& order_key,
                     const name& counterparty,
                     const string& payment_method, 
                     const asset& approved_amount);
```

#### Step 3: Off-chain swaps and approval
The buyer sends the payment or goods to the seller. This happens off-chain of course.

The buyer then calls the ```approvepay``` action mostly as a signal that they have completed their side of the order.  This step actually isn't required.

When the seller calls ```approvepay```, it signals that they have successfully received the payment, and their funds are released from escrow, funds are deducted for fees, and the remainder is sent to the buyer.

### Revenue contract
The revenue contract is a general purpose contract for many-to-many revenue management.

It should be used if there are entities that should receive fees as part of the transaction. For example, if there are liquidity pools, community managers, referrals, arbitrators, etc.  

A revenue provider, in the case above, the swaps account, establishes Trust Lines that enable revenue receivers to become eligible for disbursements. Providers then simply send tokens to the contract and also call ```paytrustline``` which allocates to the funds to a specific receiver. 

The receiver calls ```paypartner``` to request disbursement from their holdings within the contract.

### Other features
#### Cancel order
#### Volume throttling
