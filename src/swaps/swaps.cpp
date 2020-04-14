#include <swaps.hpp>

// workaround for bug on wildcard notification 
// https://github.com/EOSIO/eosio.cdt/issues/497#issuecomment-484691582
ACTION swaps::dummy () {}

ACTION swaps::reset () {
   require_auth (get_self());

   config_table config_s (get_self(), get_self().value);
   config_s.remove ();
   
   order_table order_t (get_self(), get_self().value);
   auto o_itr = order_t.begin ();
   while (o_itr != order_t.end()) {
      o_itr = order_t.erase (o_itr);
   }
}

ACTION swaps::printversion () {
   print ("Dapp v2.1\n");
}

ACTION swaps::setconfig (const name& escrow_account,
                        const name& revenue_account,
                        const float& usd_holder_perc_fee,
                        const asset& usd_holder_asset_fee,
                        const float& eos_holder_perc_fee,
                        const asset& eos_holder_asset_fee,
                        const asset& order_min,
                        const asset& order_max,
                        const asset& max_open_per_user,
                        const name& eos_token_contract) {

   require_auth (get_self());
   check ( is_account (escrow_account), "Escrow account is not a valid EOS account: " + escrow_account.to_string());
   check ( is_account (revenue_account), "Revenue account is not a valid EOS account: " + revenue_account.to_string());
   check ( is_account (eos_token_contract), "EOS token contract is not a valid EOS account: " + eos_token_contract.to_string());

   check (  usd_holder_perc_fee > 0 && usd_holder_perc_fee <= 1 && 
            eos_holder_perc_fee > 0 && eos_holder_perc_fee <= 1, "EOS and USD holder fee percentages must be between 0 and 1.");
   
   check ( usd_holder_asset_fee.symbol == common::S_USD, "USD holder asset fee must be a valid symbol of " + common::S_USD.code().to_string());

   check (  eos_holder_asset_fee.symbol == common::S_EOS &&
            order_min.symbol == common::S_EOS &&
            order_max.symbol == common::S_EOS &&
            max_open_per_user.symbol == common::S_EOS, "All EOS asset configurations must be a valid symbol of " + common::S_EOS.code().to_string());
   
   check ( order_min <= order_max, "Order minimum must be less than or equal order maximum");
   check ( order_min <= max_open_per_user, "Order minimum must be less than or equal to the maximum open per user.");
   check ( eos_holder_asset_fee <= order_min, "EOS holder fee asset must be less than order minimum.");
   
   config_table config_s (get_self(), get_self().value);
   Config c = config_s.get_or_create (get_self(), Config());
   c.escrow_account        = escrow_account;
   c.revenue_account       = revenue_account;
   c.usd_holder_perc_fee   = usd_holder_perc_fee;
   c.usd_holder_asset_fee  = usd_holder_asset_fee;
   c.eos_holder_perc_fee   = eos_holder_perc_fee;
   c.eos_holder_asset_fee  = eos_holder_asset_fee;
   c.order_min             = order_min;
   c.order_max             = order_max;
   c.max_open_per_user     = max_open_per_user;
   c.eos_token_contract    = eos_token_contract;
   config_s.set (c, get_self());
}

ACTION swaps::setthrottle (const uint16_t& global_throttle, const uint16_t& user_throttle) {

   config_table      config_s (get_self(), get_self().value);
   Config c = config_s.get_or_create (get_self(), Config());
   c.global_throttle = global_throttle;
   c.user_throttle = user_throttle;
   config_s.set (c, get_self());
}

ACTION swaps::create (const name& account, 
                     const uint8_t& order_type, 
                     const asset& amount, 
                     const asset& min_trx,
                     const string& price_type, 
                     const asset& price_per_eos,
                     const float& price_var, 
                     const vector<string> payment_methods,
                     const name& app_account ) {

   require_auth (account); 
   config_table config_s (get_self(), get_self().value);
   Config c = config_s.get ();

   check ( is_account(app_account), "Account is not an EOS account: " + app_account.to_string());
   check ( is_account (account), "Creator account is not a valid EOS account: " + account.to_string());
   check ( amount.symbol == common::S_EOS, 
         "EOS amount to sell must use the exact EOS symbol, e.g. \"100.0000 EOS\"" + amount.symbol.code().to_string());
   check ( amount <= c.order_max, "Order amount is greater than the allowed maximum. Maximum: " + c.order_max.to_string());
   check ( amount >= c.order_min, "Order amount is less than the allowed minimum. Minimum: " + c.order_min.to_string());
   check ( min_trx <= c.order_max, "Transaction minimum amount is greater than the allowed maximum. Maximum: " + c.order_max.to_string());
   check ( min_trx >= c.order_min, "Transaction minimum amount is less than the allowed minimum. Minimum: " + c.order_min.to_string());
   check ( price_per_eos.symbol == common::S_USD, "Price per EOS must be specified as \"8.50 USD\" : " + common::S_USD.code().to_string());
   check ( price_var > -1 && price_var < 1, "Price variance must be between -1 and 1. Your value: " + std::to_string (price_var));
   check ( price_type == common::EXACT_PRICE || price_type == common::MARKET_VARIANCE, "Invalid value for price type. Your value: " + price_type);
   check ( order_type == 1 || order_type == 2, "Price type must be 1 or 2. Your value: " + std::to_string(order_type));

   confirm_balance (account, amount);
  
   order_table order_t (get_self(), get_self().value);
   order_t.emplace (get_self(), [&](auto &order) {
      order.creator           = account;
      order.app_account       = app_account;
      order.orig_eos_amount   = amount;
      order.eos_amount        = amount;
      order.order_type        = order_type;
      order.price_type        = price_type;
      order.price_per_eos     = price_per_eos;
      order.price_var         = price_var;
      order.payment_methods   = payment_methods;
      order.min_trx           = min_trx;
      order.created_date      = current_block_time().to_time_point().sec_since_epoch();
      order.updated_date      = current_block_time().to_time_point().sec_since_epoch();

      string stringKey = std::to_string (order.created_date) 
         + std::to_string(order.creator.value) + std::to_string(order.eos_amount.amount) + std::to_string(order.order_type);

      order.order_key = hash (stringKey);
    });
}

ACTION swaps::accept (const uint64_t& order_key,
                     const name& counterparty,
                     const string& payment_method, 
                     const asset& approved_amount) {

   require_auth (counterparty);
   check (is_account(counterparty), "Counter party is not a valid EOS account : " + counterparty.to_string());
 
   order_table order_t (get_self(), get_self().value);
   auto order_itr = order_t.find (order_key);
   check (order_itr != order_t.end(), "Order not found: " + std::to_string (order_key));

   check (order_itr->creator != counterparty, "Cannot accept you own order: " + order_itr->creator.to_string());
   check (order_itr->counter_party.value == 0, "This order has already been accepted: " + order_itr->counter_party.to_string());
   check (order_itr->order_status == common::OPEN, "Order is not open. Order status: " + order_itr->order_status);
   check (approved_amount >= order_itr->min_trx, "Order acceptance amount must be greater than this order's minimum transaction amount: " + order_itr->min_trx.to_string());

   bool valid_payment_method = false;
   for(auto i : order_itr->payment_methods) {
      if (i == payment_method) {
         valid_payment_method = true;
         break;
      }
   }
   check (valid_payment_method, "Counter party must choose a method on the order. Invalid payment method: " + payment_method);

   print (" Accepted order: ", std::to_string(order_key), "\n");
   print (" Approved amount: ", approved_amount, "\n");
   print (" EOS Amount: ", order_itr->eos_amount, "\n");

   order_t.modify (order_itr, get_self(), [&](auto &order) {
     
      // handle partial orders
      if (approved_amount < order_itr->eos_amount) {
         print (" Calling create partial order\n");
         create_partial_order (order, order.eos_amount - approved_amount);
         order.eos_amount = approved_amount;
      } 
            
      // handler market variance orders
      if (order_itr->price_type == common::MARKET_VARIANCE) {
         order.price_per_eos = adjust_asset(get_eosusd(), 1 + order_itr->price_var);
      }
    
      // BUY ORDERS - check balance
      if (order_itr->order_type == common::BUY) {
         confirm_balance (counterparty, approved_amount);
      }
      // SELL ORDERS - check balance
      else if (order_itr->order_type == common::SELL) {
         // double check seller's available funds
         // should never trigger error, or else something else is wrong
         confirm_balance (order_itr->creator, approved_amount);

      } else {
         check (false, "Order type not supported.");
      }

      order.order_status = common::ESCROW;
      order.counter_party = counterparty;

      print (" Approved Amount   : ", std::to_string(approved_amount.amount), "\n");
      print (" Price per EOS Amount : ", std::to_string(order_itr->price_per_eos.amount), "\n");
      print (" Order Value Amount   : ", std::to_string((float) approved_amount.amount * (float) order_itr->price_per_eos.amount ));

      order.order_value_usd = asset { static_cast<int64_t>((float)approved_amount.amount / 10'000 * (float) order_itr->price_per_eos.amount), common::S_USD };
      order.eos_holder_fee = calculate_eos_holder_fee (order);

      asset usd_fee = calculate_usd_holder_fee (order);
      print ( "\n\nUSD Fee calculated  : ", usd_fee, "\n\n");
      order.usd_to_send = order.order_value_usd + usd_fee;
      order.usd_holder_fee = usd_fee;
      order.usd_holder_fee_in_eos = usd_to_eos (usd_fee);

      order.eos_usd_rate_for_fees = get_eosusd ();
      
   });
}

ACTION swaps::approvepay (const uint64_t& order_key,
                           const name& approver) {

   require_auth (approver);

   print (" Approving pay for : " + std::to_string(order_key) + " by " + approver.to_string(), "\n");

   order_table       order_t (get_self(), get_self().value);
   auto o_itr = order_t.find (order_key);
   check (o_itr != order_t.end(), "Order with order key not found:" + std::to_string(order_key));

   config_table      config_s (get_self(), get_self().value);
   Config c = config_s.get_or_create (get_self(), Config());
   check (o_itr->creator_approved_pay <= current_block_time().to_time_point().sec_since_epoch(), "Invalid creator approved pay date.");
   check (o_itr->counterparty_approved_pay <= current_block_time().to_time_point().sec_since_epoch(), "Invalid creator approved pay date.");

   check (o_itr->order_status == common::ESCROW, "Invalid order status. Only orders in escrow can be approved for payment. Curren status: " + o_itr->order_status);
   
   string fulfill_memo { "Fulfillment of Swap Order Number : " + std::to_string (o_itr->order_key) + "; Approved by: " + approver.to_string()};
   asset fees_for_swaps = o_itr->eos_holder_fee + o_itr->usd_holder_fee_in_eos;
   asset amount_for_buyer = o_itr->eos_amount - fees_for_swaps;
   
   order_t.modify (o_itr, get_self(), [&](auto &o) {
      if (o_itr->creator == approver && o_itr->order_type == common::SELL) {
         print (" Seller=Creator has approved, release from escrow   " + approver.to_string());
         o.creator_approved_pay = current_block_time().to_time_point().sec_since_epoch();
         o.eos_fulfilled = amount_for_buyer;
         sendfrombal (o_itr->creator, o_itr->counter_party, amount_for_buyer, fulfill_memo);
         process_fee (o_itr->order_key, o_itr->creator, o_itr->app_account, c.eos_token_contract, fees_for_swaps);
         o.order_status = common::FULFILLED;
      } else if (o_itr->creator == approver && o_itr->order_type == common::BUY) {
         //check (false, "Attempted approver is the creator of a Buy order. The seller must approve after the buyer transfers funds.");
         o.creator_approved_pay = current_block_time().to_time_point().sec_since_epoch();
         o.order_status = common::BUYER_APPROVED;
      } else if (o_itr->counter_party == approver && o_itr->order_type == common::SELL) {
         //check (false, "Attempted approver is the counter-party of a Sell order. The seller must approve after the buyer transfers funds.");
         o.counterparty_approved_pay = current_block_time().to_time_point().sec_since_epoch();
         o.updated_date = current_block_time().to_time_point().sec_since_epoch();
         o.order_status = common::BUYER_APPROVED;
      } else if (o_itr->counter_party == approver && o_itr->order_type == common::BUY) {
         o.counterparty_approved_pay = current_block_time().to_time_point().sec_since_epoch();
         o.eos_fulfilled = amount_for_buyer;
         sendfrombal (o_itr->counter_party, o_itr->creator, amount_for_buyer, fulfill_memo);
         process_fee (o_itr->order_key, o_itr->counter_party, o_itr->app_account, c.eos_token_contract, fees_for_swaps);
         o.order_status = common::FULFILLED;
      }
      
      //o.creator_approved_pay = current_block_time().to_time_point().sec_since_epoch();
      o.order_status = common::FULFILLED;

   });
}

ACTION swaps::cancel (const uint64_t& order_key) {

   order_table order_t (get_self(), get_self().value);
   auto o_itr = order_t.find (order_key);
   string error {"Order with order key not found : " + std::to_string(order_key) };
   check (o_itr != order_t.end(), error);

   require_auth (o_itr->creator);

   string s { "Only orders with an OPEN status can be cancelled, status : " + o_itr->order_status };
   check (o_itr->order_status == common::OPEN, s);

   string memo { "Swap: Order cancel and return of funds. Order key: " + std::to_string(o_itr->order_key) };
   if (o_itr->order_type == common::SELL) {  // creator is the seller
      sendfrombal (o_itr->creator, o_itr->creator, o_itr->eos_amount, memo);
   } 

   order_t.erase (o_itr);
}

ACTION swaps::prune (const uint64_t& days_to_keep) {
   require_auth (get_self());
}

void swaps::deposit ( const name& from, const name& to, const asset& quantity, const string& memo ) {

   //eosio::check (!is_paused(), "Contract is paused - no actions allowed.");
   if (to != get_self()) { return; }  // not sending to treasury

   config_table      config_s (get_self(), get_self().value);
   Config c = config_s.get_or_create (get_self(), Config());
   check (quantity.symbol == common::S_EOS, "Only deposits of EOS are accepted.");
  
   check (get_first_receiver() == c.eos_token_contract,  
      "Only deposits of the configured EOS token contract are accepted. Configured contract: " + c.eos_token_contract.to_string());
  
   balance_table balances(get_self(), from.value);
   asset new_balance;
   auto it = balances.find(quantity.symbol.code().raw());
   if(it != balances.end()) {
      check (it->token_contract == get_first_receiver(), "Transfer does not match existing token contract.");
      balances.modify(it, get_self(), [&](auto& bal){
         // Assumption: total currency issued by eosio.token will not overflow asset
         bal.funds += quantity;
         new_balance = bal.funds;
      });
   }
   else {
      balances.emplace(get_self(), [&](auto& bal){
         bal.funds = quantity;
         bal.token_contract  = get_first_receiver();
         new_balance = quantity;
      });
   }

   print ("\n");
   print(name{from}, " deposited:       ", quantity, "\n");
   print(name{from}, " funds available: ", new_balance);
   print ("\n");
}
