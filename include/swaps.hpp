#ifndef SWAP_H
#define SWAP_H
#include <eosio/eosio.hpp>
#include <eosio/symbol.hpp>
#include <eosio/asset.hpp>
#include <eosio/singleton.hpp>
#include <eosio/symbol.hpp>
#include <eosio/crypto.hpp>
#include <eosio/multi_index.hpp>
#include <eosio/system.hpp>
#include <eosio/print.hpp>

#include "common.hpp"
#include <string>

using namespace std;
using std::string;
using std::vector;
using namespace eosio;

CONTRACT swaps : public contract {
   public:
      using contract::contract;

      struct [[ eosio::table, eosio::contract("swaps") ]] Config 
      {
         name           escrow_account          ;
         name           revenue_account         ;
         float          usd_holder_perc_fee     = 0.000000000000;
         asset          usd_holder_asset_fee    = asset { 0, common::S_USD };
         float          eos_holder_perc_fee     = 0.000000000000;
         asset          eos_holder_asset_fee    = asset { 0, common::S_EOS };
         float          default_app_fee_share   = 0.00000000000;
         asset          order_min               = asset { 0, common::S_EOS };
         asset          order_max               = asset { 0, common::S_EOS };
         uint16_t       global_throttle         = 0;
         uint16_t       user_throttle           = 0;
         asset          max_open_per_user       = asset { 0, common::S_EOS };
         name           eos_token_contract      = "eosio.token"_n ;
      };

      struct [[ eosio::table, eosio::contract("swaps") ]] App 
      {
         name           app_account          ;
         string         description          ;
         float          fee_share            = 0.0000000000;

         uint64_t       referrals            = 0;
         asset          referral_amount      = asset { 0, common::S_EOS };
         
         uint32_t       created_date         = current_block_time().to_time_point().sec_since_epoch();
         uint32_t       updated_date         = current_block_time().to_time_point().sec_since_epoch();
         
         uint64_t       primary_key()        { return app_account.value; }
      }; 

      // struct [[ eosio::table, eosio::contract("swaps") ]] Profile
      // {
         //   uint32_t       created_date               = current_block_time().to_time_point().sec_since_epoch();
        // uint32_t       updated_date               = current_block_time().to_time_point().sec_since_epoch();
      // }

      // struct [[ eosio::table, eosio::contract("swaps") ]] Review 
      // {
//  uint32_t       created_date               = current_block_time().to_time_point().sec_since_epoch();
//          uint32_t       updated_date               = current_block_time().to_time_point().sec_since_epoch();
      // }


      struct [[ eosio::table, eosio::contract("swaps") ]] Order 
      {
         uint64_t       order_key                  ;
         name           creator                    ;
         name           counter_party              ;
         name           app_account                ;
         asset          orig_eos_amount            = asset { 0, common::S_EOS };
         asset          eos_amount                 = asset { 0, common::S_EOS };   
         asset          order_value_usd            = asset { 0, common::S_USD };
         asset          min_trx                    = asset { 0, common::S_EOS };
         asset          eos_holder_fee             = asset { 0, common::S_EOS };
         asset          usd_holder_fee             = asset { 0, common::S_EOS };
         asset          usd_holder_fee_in_eos      = asset { 0, common::S_EOS };
         asset          usd_to_send                = asset { 0, common::S_USD };
         asset          eos_usd_rate_for_fees      = asset { 0, common::S_USD };
         asset          eos_fulfilled              = asset { 0, common::S_EOS };

         uint8_t        order_type                 = common::BUY;
         string         price_type                 = common::EXACT_PRICE;

         asset          price_per_eos              = asset { 0, common::S_USD };  
         float          price_var                  = 0.0000000000;

         vector<string> payment_methods            ;

         string         order_status               = common::OPEN;
         uint32_t       created_date               = current_block_time().to_time_point().sec_since_epoch();
         uint32_t       updated_date               = current_block_time().to_time_point().sec_since_epoch();
         uint32_t       creator_approved_pay       = 0;
         uint32_t       counterparty_approved_pay  = 0;

         uint64_t       by_creator()      const { return creator.value; }
         uint64_t       by_counterparty() const { return counter_party.value; }
         uint64_t       by_appaccount()   const { return app_account.value; }
         uint64_t       by_created_date() const { return created_date; }
         uint64_t       by_price()        const { return price_per_eos.amount; }
         uint64_t       by_amount()       const { return eos_amount.amount; }
         uint64_t       by_order_type()   const { return order_type; }
         // int64_t        by_price_var()    const { return price_var * 1000000; }
         uint64_t       primary_key()     const { return order_key; }
      };

      typedef multi_index<"orders"_n, Order,
         indexed_by<"bycreator"_n,
            const_mem_fun<Order, uint64_t, &Order::by_creator>>,
         indexed_by<"bycreated"_n,
            const_mem_fun<Order, uint64_t, &Order::by_created_date>>,
         indexed_by<"byprice"_n, 
            const_mem_fun<Order, uint64_t, &Order::by_price>>,
         indexed_by<"byamount"_n, 
            const_mem_fun<Order, uint64_t, &Order::by_amount>>,
         indexed_by<"byordertype"_n, 
            const_mem_fun<Order, uint64_t, &Order::by_order_type>>,
         indexed_by<"bycounter"_n, 
            const_mem_fun<Order, uint64_t, &Order::by_counterparty>>
      > order_table;

      TABLE balance 
      {
         asset funds;
         name token_contract;
         uint64_t primary_key() const { return funds.symbol.code().raw(); }
      };

      typedef singleton<"configs"_n, Config> config_table;
      typedef multi_index<"configs"_n, Config> config_table_placehoder;
      typedef multi_index<"balances"_n, balance> balance_table;
      typedef multi_index<"apps"_n, App> app_table;
            
      ACTION reset () ;
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

      ACTION setthrottle (const uint16_t& global_throttle, const uint16_t& user_throttle);

      ACTION create (const name& account, 
                     const uint8_t& order_type, 
                     const asset& amount, 
                     const asset& min_trx,
                     const string& price_type, 
                     const asset& price_per_eos,
                     const float& price_var, 
                     const vector<string> payment_methods,
                     const name& app_account);

      ACTION accept (const uint64_t& order_key,
                     const name& counterparty,
                     const string& payment_method, 
                     const asset& approved_amount);

      ACTION approvepay (const uint64_t& order_key,
                           const name& approver);

      ACTION cancel (const uint64_t& order_key);

      ACTION prune (const uint64_t& days_to_keep);

      ACTION printversion ();

      // workaround for bug on wildcard notification 
      // https://github.com/EOSIO/eosio.cdt/issues/497#issuecomment-484691582
      [[eosio::on_notify("dummy::dummy")]]
      void dummy ();
      using dummy_action = eosio::action_wrapper<"dummy"_n, &swaps::dummy>;

      [[eosio::on_notify("*::transfer")]]
      void deposit ( const name& from, const name& to, const asset& quantity, const string& memo );
      using transfer_action = action_wrapper<name("transfer"), &swaps::deposit>;

      using create_action = action_wrapper<"create"_n, &swaps::create>;
      using setconfig_action = action_wrapper<"setconfig"_n, &swaps::setconfig>;
      using setthrottle_action = action_wrapper<"setthrottle"_n, &swaps::setthrottle>;
      using accept_action = action_wrapper<"accept"_n, &swaps::accept>;
      using approvepay_action = action_wrapper<"approvepay"_n, &swaps::approvepay>;
      using cancel_action = action_wrapper<"cancel"_n, &swaps::cancel>;
      using prune_action = action_wrapper<"prune"_n, &swaps::prune>;

   private:

      uint64_t hash(std::string str) {
         uint64_t id = 0;
         checksum256 h = sha256(const_cast<char*>(str.c_str()), str.size());
         auto hbytes = h.extract_as_byte_array();
         for(int i=0; i<4; i++) {
               id <<=8;
               id |= hbytes[i];
         }
         return id;
      }

      asset adjust_asset (const asset& original_asset, const float& adjustment) {
         return asset { static_cast<int64_t> (original_asset.amount * adjustment), original_asset.symbol };
      }

      // returns the new partial order and updates existing order
      void create_partial_order (Order &existing_order, const asset& new_order_amount) {
         print (" Creating partial order  : ", new_order_amount, "\n");
         uint32_t updated_date = current_block_time().to_time_point().sec_since_epoch();

         print (" Existing order: ", std::to_string(existing_order.order_key));

         order_table order_t (get_self(), get_self().value);
         order_t.emplace (get_self(), [&](auto& o) {
            o.eos_amount         = new_order_amount;
            print (" New Order Amount  : ", new_order_amount, "\n");
            o.orig_eos_amount    = existing_order.orig_eos_amount;
            o.creator            = existing_order.creator;
            o.order_type         = existing_order.order_type;
            o.min_trx            = existing_order.min_trx;
            o.price_type         = existing_order.price_type;
            o.price_per_eos      = existing_order.price_per_eos;
            print (" Existing price per eos: ", existing_order.price_per_eos, "\n");
            o.price_var          = existing_order.price_var;
            o.payment_methods    = existing_order.payment_methods;
            o.created_date       = existing_order.created_date;
            o.updated_date       = updated_date;

            string stringKey = std::to_string (updated_date)
            + std::to_string(o.creator.value) 
            + std::to_string(new_order_amount.amount) 
            + std::to_string(o.order_type);

            print (" New order stringKey: ", stringKey, "\n");
            uint64_t hashed_key = hash (stringKey);
            print (" New hashed key: ", std::to_string(hashed_key), "\n");
      
            o.order_key = hash (stringKey);
         });

         print (" Saved new order\n\n");
      }

      asset usd_to_eos (const asset& usd_asset) {
         asset eosusd = get_eosusd ();
         print (" EOSUSD            : ", eosusd, "\n");
         print (" EOSUSD Amount     : ", std::to_string (eosusd.amount), "\n");
         print (" USD Asset Amount  : ", std::to_string (usd_asset.amount), "\n");
         float converted_float = ((float) usd_asset.amount / (float) eosusd.amount) ;
         print (" Converted Float   : ", std::to_string (converted_float), "\n");
         asset eos_asset = adjust_asset(common::ONE_EOS, converted_float);
         //asset eos_asset = asset { static_cast<int64_t>((float) usd_asset.amount / (float) (get_eosusd ().amount)) * 100, common::S_EOS };
        // EMIT_TO_EOS_EVENT (usd_asset, eosusd, eos_asset);
        print ( " TO EOS\n");
        print ( " USD asset   : ", usd_asset, "\n");
        print ( " EOS asset   : ", eos_asset, "\n");
         return eos_asset;
      }

      asset eos_to_usd (const asset& eos_asset) {
         asset eosusd = get_eosusd ();
         //asset usd_asset = asset { static_cast<int64_t>((float) eos_asset.amount * (float) get_eosusd().amount / 100), common::S_USD };
        // EMIT_TO_USD_EVENT (eos_asset, eosusd, usd_asset);
        float converted_float = ((float) eos_asset.amount * (float) eosusd.amount );
        asset usd_asset = adjust_asset (common::ONE_USD, converted_float);
        print ( " TO USD\n");
        print ( " EOS asset   : ", eos_asset, "\n");
        print ( " USD asset   : ", usd_asset, "\n");
         return usd_asset;
      }

      asset calculate_eos_holder_fee (const Order& order) {
         config_table      config_s (get_self(), get_self().value);
         Config c = config_s.get_or_create (get_self(), Config());

         asset fee_perc_asset = adjust_asset (order.eos_amount, c.eos_holder_perc_fee);
         print (" EOS Holder fee \n\n fee_per_asset: ", fee_perc_asset, "\n");
         print (" Config asset minimum : ", c.eos_holder_asset_fee, "\n");
         print (" Max amount: ", std::to_string( std::max(fee_perc_asset.amount, c.eos_holder_asset_fee.amount)), "\n" );
         asset fee = asset { std::max (fee_perc_asset.amount, c.eos_holder_asset_fee.amount), common::S_EOS };
         return fee;
      }

      asset calculate_usd_holder_fee (const Order& order) {
         config_table      config_s (get_self(), get_self().value);
         Config c = config_s.get_or_create (get_self(), Config());

         asset fee_perc_asset = adjust_asset (order.order_value_usd, c.usd_holder_perc_fee);
         asset fee = asset { std::max (fee_perc_asset.amount, c.usd_holder_asset_fee.amount), common::S_USD };
         return fee;
      }

      void confirm_balance (const name& account, 
                            const asset& amount) {

         balance_table balances(get_self(), account.value);
         auto b_itr = balances.find(common::S_EOS.code().raw());
         check (b_itr != balances.end(), "No balance exists in Swap contract. account: " + account.to_string() +
            amount.symbol.code().to_string());
         check (b_itr->funds >= amount, "Insufficient funds in Swap contract. account: " + account.to_string() + " has balance of " +
            b_itr->funds.to_string() + ".  Transaction requires: " + amount.to_string());
      }

      void process_fee (   const uint64_t& order_key,
                           const name& fee_payer, 
                           const name& referrer,
                           const name& token_contract,
                           const asset& fee) {

         config_table      config_s (get_self(), get_self().value);
         Config c = config_s.get_or_create (get_self(), Config());
         
         string fees_memo { "Fees for Swap Order Number : " + std::to_string (order_key)};
         sendfrombal (fee_payer, c.revenue_account, fee, fees_memo);
         
         if ( is_account(referrer)) {
            print (" Invoking inline action to pay trustline, referrer   : " + referrer.to_string(), "\n");
            action(
               permission_level{get_self(), "active"_n},
               c.revenue_account, "paytrustline"_n,
               std::make_tuple(get_self(), referrer, token_contract, fee, fees_memo))
            .send();
         }
      }
    

      void sendfrombal (const name from,
                        const name to,
                        const asset token_amount,
                        const string memo) 
      {
         if (token_amount.amount > 0) {   
               balance_table bal_table (get_self(), from.value);
               auto it = bal_table.find(token_amount.symbol.code().raw());
               eosio::check (it != bal_table.end(), "Sender does not have a balance within the contract." );
               eosio::check (it->funds >= token_amount, "Insufficient balance.");

               bool remove_record = false;
               bal_table.modify (it, get_self(), [&](auto &b) {
                  if (b.funds == token_amount) {
                     remove_record = true;
                  }
                  b.funds -= token_amount;
               });

               require_recipient (from);

               payeos (get_self(), to, token_amount, memo);     
                  
               if (remove_record) {
                  bal_table.erase (it);
               }
         }  
      }

      void payeos ( const name from,
                    const name to,
                    const asset token_amount,
                    const string memo)
      {
         config_table config_s (get_self(), get_self().value);
         Config c = config_s.get_or_create (get_self(), Config());

         print("---------- Payment -----------\n");
         print("Token Contract   : ", name{c.eos_token_contract}, "\n");
         print("From             : ", name{from}, "\n");
         print("To               : ", name{to}, "\n");
         print("Amount           : ", token_amount, "\n");
         print("Memo             : ", memo, "\n");

         action(
               permission_level{from, "active"_n},
               c.eos_token_contract, "transfer"_n,
               std::make_tuple(from, to, token_amount, memo))
               .send();

         print("---------- End Payment -------\n");
      }

      struct [[eosio::table]] datapoints {
         uint64_t id;
         name     owner; 
         uint64_t value;
         uint64_t median;
         uint64_t timestamp;

         uint64_t primary_key() const {return id;}
         uint64_t by_timestamp() const {return timestamp;}
         uint64_t by_value() const {return value;}
      };

      typedef eosio::multi_index<"datapoints"_n, datapoints,
            indexed_by<"value"_n, const_mem_fun<datapoints, uint64_t, &datapoints::by_value>>, 
      indexed_by<"timestamp"_n, const_mem_fun<datapoints, uint64_t, &datapoints::by_timestamp>>> datapointstable;

      asset get_eosusd () {
         datapointstable d_t ("delphioracle"_n, "eosusd"_n.value);
         auto d_itr = d_t.begin();
         //return asset { 645, common::S_USD };
         return asset { static_cast<int64_t>( (float)d_itr->value / (float) 100), common::S_USD};
      }

};


#endif