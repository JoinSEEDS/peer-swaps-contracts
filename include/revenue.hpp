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

CONTRACT revenue : public contract
{
public:
  using contract::contract;

  // struct [[ eosio::table, eosio::contract("revenue") ]] Config
  // {
  //   float default_app_fee_share = 0.00000000000;
  //   // vector<asset>  whitelist_assets        ;
  // };

  struct [[ eosio::table, eosio::contract("revenue") ]] Provider
  {
    name      provider_account;
    string    description;
    // asset     provided_amount;
    //uint64_t  trx_count;

    // uint32_t  created_date = current_block_time().to_time_point().sec_since_epoch();
    // uint32_t  updated_date = current_block_time().to_time_point().sec_since_epoch();

    uint64_t  primary_key()   const { return provider_account.value; }
  };

  struct [[ eosio::table, eosio::contract("revenue") ]] Receiver
  {
    name      receiver_account;
    string    description;
   // uint64_t  trx_count         = 0;

   // uint32_t  created_date       = current_block_time().to_time_point().sec_since_epoch();
   // uint32_t  updated_date       = current_block_time().to_time_point().sec_since_epoch();

    uint64_t primary_key()    const { return receiver_account.value; }
  };

  // provider should be used as the table scope
  struct [[ eosio::table, eosio::contract("revenue") ]] Trustline
  {
    uint64_t  trustline_id      = 0;
    name      receiver_account  ;
    //name      provider_account  ;
    uint64_t  trx_count         = 0;
    float     rev_share         = 0.0000000000;
    string    notes             ;
    uint32_t  created_date       = current_block_time().to_time_point().sec_since_epoch();
    uint32_t  updated_date       = current_block_time().to_time_point().sec_since_epoch();

    uint64_t  primary_key()  const { return trustline_id; }
    uint64_t  by_receiver()  const { return receiver_account.value; }
  };

  // this table will be scoped with the asset symbol
  struct [[ eosio::table, eosio::contract("revenue")]] TrustlineAsset
  {
    uint64_t  trustline_asset_id    = 0;
    uint64_t  trustline_id          = 0;
    asset     amount_in             ;
    asset     amount_out            ;
    name      token_contract        ;
  
    uint64_t  by_trstlineid() const { return trustline_id; }
    uint64_t  primary_key()   const { return trustline_asset_id; }
  };

  // this table uses account name as the scope and symbol as the primary key
  struct [[ eosio::table, eosio::contract("revenue")]] BalanceDue
  {
    asset     amount_due            ;
    name      token_contract        ;
    uint32_t  last_paid_date        = 0;
    string    memo                  ;
    uint64_t  primary_key()   const { return amount_due.symbol.code().raw(); }
  };

  struct [[ eosio::table, eosio::contract("revenue") ]] Blacklist
  {
    name      account;
    uint32_t  created_date       = current_block_time().to_time_point().sec_since_epoch();
    uint32_t  updated_date       = current_block_time().to_time_point().sec_since_epoch();
    uint64_t  primary_key() const { return account.value; }
  };

  struct [[ eosio::table, eosio::contract("revenue") ]] Receipt
  {
    uint64_t  receipt_id      = 0;
    name      revenue_from    ;
    asset     revenue_amount  ;
    name      token_contract  ;
    string    memo            ;
    uint32_t  receipt_date    = current_block_time().to_time_point().sec_since_epoch();

    uint64_t primary_key() const { return receipt_id; }
  };

  // typedef singleton<"configs"_n, Config> config_table;
  // typedef multi_index<"configs"_n, Config> config_table_placehoder;
  typedef multi_index<"providers"_n, Provider> provider_table;
  typedef multi_index<"receivers"_n, Receiver> receiver_table;

  typedef multi_index<"trustlines"_n, Trustline,
    indexed_by<"byreceiver"_n, 
      const_mem_fun<Trustline, uint64_t, &Trustline::by_receiver>>
  > trustline_table;

  typedef multi_index<"blacklists"_n, Blacklist> blacklist_table;
  typedef multi_index<"receipts"_n, Receipt> receipt_table;
  
  typedef multi_index<"trstassets"_n, TrustlineAsset,
    indexed_by<"bytrstlineid"_n, 
      const_mem_fun<TrustlineAsset, uint64_t, &TrustlineAsset::by_trstlineid>>
  > trstlinasset_table;

  typedef multi_index<"balancesdue"_n, BalanceDue> balances_due_table;

  ACTION reset();

  // ACTION setconfig(const float &default_app_fee_share);
  // const vector<asset> whitelist_assets);

  ACTION addprovider ( const name &provider_account, const string& description);
  ACTION remprovider ( const name &provider_account);

  ACTION addreceiver(const name& receiver_account, const string& description);
  ACTION remreceiver(const name& receiver_account);

  ACTION addtrustline ( const name& provider_account, const name& receiver_account, const float &rev_share, const string& notes);
  ACTION remtrustline (const name &receiver_account, const name &provider_account);
  ACTION updtrustline(const name &provider_account, const name &receiver_account, const float &rev_share, const string &notes);

  ACTION paytrustline (const name &provider_account, 
                            const name &receiver_account, 
                            const name& token_contract, 
                            const asset &total_fee, 
                            const string& memo);

  ACTION paypartner (const name& receiver_account, 
                      const asset& symbol_to_pay );

  ACTION blacklist(const name &receiver_account);
  ACTION whitelist(const name &receiver_account);

  // workaround for bug on wildcard notification
  // https://github.com/EOSIO/eosio.cdt/issues/497#issuecomment-484691582
  [[eosio::on_notify("dummy::dummy")]] void dummy();
  using dummy_action = eosio::action_wrapper<"dummy"_n, &revenue::dummy>;

  [[eosio::on_notify("*::transfer")]] void receive(const name &from, const name &to, const asset &quantity, const string &memo);
  using transfer_action = action_wrapper<name("transfer"), &revenue::receive>;

  // using setconfig_action = action_wrapper<"setconfig"_n, &revenue::setconfig>;

  using addprovider_action = action_wrapper<"addprovider"_n, &revenue::addprovider>;
  using remprovider_action = action_wrapper<"remprovider"_n, &revenue::remprovider>;

  using addreceiver_action = action_wrapper<"addreceiver"_n, &revenue::addreceiver>;
  using remreceiver_action = action_wrapper<"remreceiver"_n, &revenue::remreceiver>;

  using addtrustline_action = action_wrapper<"addtrustline"_n, &revenue::addtrustline>;
  using remtrustline_action = action_wrapper<"remtrustline"_n, &revenue::remtrustline>;

  using updtrustline_action = action_wrapper<"updtrustline"_n, &revenue::updtrustline>;
  using paytrustline_action = action_wrapper<"paytrustline"_n, &revenue::paytrustline>;
  using paypartner_action = action_wrapper<"paypartner"_n, &revenue::paypartner>;

  using blacklist_action = action_wrapper<"blacklist"_n, &revenue::blacklist>;
  using whitelist_action = action_wrapper<"whitelist"_n, &revenue::whitelist>;

private:
  asset adjust_asset(const asset &original_asset, const float &adjustment)
  {
    return asset{static_cast<int64_t>(original_asset.amount * adjustment), original_asset.symbol};
  }

  void paytoken(const name token_contract,
                const name from,
                const name to,
                const asset token_amount,
                const string memo)
  {
    print("---------- Payment -----------\n");
    print("Token Contract   : ", name{token_contract}, "\n");
    print("From             : ", name{from}, "\n");
    print("To               : ", name{to}, "\n");
    print("Amount           : ", token_amount, "\n");
    print("Memo             : ", memo, "\n");

    action(
        permission_level{from, "active"_n},
        token_contract, "transfer"_n,
        std::make_tuple(from, to, token_amount, memo))
        .send();

    print("---------- End Payment -------\n");
  }

  void add_to_balance (const name& provider_account,
                        const name& receiver_account, 
                        const name& token_contract, 
                        const asset& amount, 
                        const string& memo) {
    require_auth (provider_account);

    print (" Adding to amount due: ", receiver_account, "\n Amount: ", amount, "\n");
    print (" Key  : ", std::to_string( amount.symbol.code().raw()), "\n");
    balances_due_table bd_t (get_self(), receiver_account.value);
    auto bd_itr = bd_t.find (amount.symbol.code().raw());
    if (bd_itr == bd_t.end()) {
      bd_t.emplace (provider_account, [&](auto& bd) {
        bd.token_contract = token_contract;
        bd.amount_due = amount;
        bd.memo = memo;
      });
    } else {
      bd_t.modify (bd_itr, provider_account, [&](auto& bd) {
        check (bd.token_contract == token_contract, "Invalid token contract; does not match contract on record.");
        bd.amount_due += amount;
        bd.memo += "; " + memo;
      });
    }
  }
};


#endif