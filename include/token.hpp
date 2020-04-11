#pragma once

#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>

#include <math.h>
#include <string>

using namespace eosio;
using std::string;

/**
* @defgroup eosiotoken eosio.token
* @ingroup eosiocontracts
*
* eosio.token contract
*
* @details eosio.token contract defines the structures and actions that allow users to create, issue, and manage
* tokens on eosio based blockchains.
*
* Includes customizations for handling splitting of rewards amongst token holders.
*
* @{
*/
class [[eosio::contract("token")]] token : public contract {
    public:
        using contract::contract;

        /**
         * Create action.
         *
         * @details Allows `issuer` account to create a token in supply of `maximum_supply`.
         * @param issuer - the account that creates the token,
         * @param maximum_supply - the maximum supply set for the token created.
         *
         * @pre Token symbol has to be valid,
         * @pre Token symbol must not be already created,
         * @pre maximum_supply has to be smaller than the maximum supply allowed by the system: 1^62 - 1.
         * @pre Maximum supply must be positive;
         *
         * If validation is successful a new entry in statstable for token symbol scope gets created.
         */
        [[eosio::action]]
        void create( const name&   issuer,
                    const asset&  maximum_supply);
        /**
         * Issue action.
         *
         * @details This action issues to `to` account a `quantity` of tokens.
         *
         * @param to - the account to issue tokens to, it must be the same as the issuer,
         * @param quntity - the amount of tokens to be issued,
         * @memo - the memo string that accompanies the token issue transaction.
         */
        [[eosio::action]]
        void issue( const name& to, const asset& quantity, const string& memo );

        /**
         * Retire action.
         *
         * @details The opposite for create action, if all validations succeed,
         * it debits the statstable.supply amount.
         *
         * @param quantity - the quantity of tokens to retire,
         * @param memo - the memo string to accompany the transaction.
         */
        [[eosio::action]]
        void retire( const asset& quantity, const string& memo );

        /**
         * Transfer action.
         *
         * @details Allows `from` account to transfer to `to` account the `quantity` tokens.
         * One account is debited and the other is credited with quantity tokens.
         *
         * @param from - the account to transfer from,
         * @param to - the account to be transferred to,
         * @param quantity - the quantity of tokens to be transferred,
         * @param memo - the memo string to accompany the transaction.
         */
        [[eosio::action]]
        void transfer( const name&    from,
                        const name&    to,
                        const asset&   quantity,
                        const string&  memo );
        /**
         * Open action.
         *
         * @details Allows `ram_payer` to create an account `owner` with zero balance for
         * token `symbol` at the expense of `ram_payer`.
         *
         * @param owner - the account to be created,
         * @param symbol - the token to be payed with by `ram_payer`,
         * @param ram_payer - the account that supports the cost of this action.
         *
         * More information can be read [here](https://github.com/EOSIO/eosio.contracts/issues/62)
         * and [here](https://github.com/EOSIO/eosio.contracts/issues/61).
         */
        [[eosio::action]]
        void open( const name& owner, const symbol& symbol, const name& ram_payer );

        /**
         * Close action.
         *
         * @details This action is the opposite for open, it closes the account `owner`
         * for token `symbol`.
         *
         * @param owner - the owner account to execute the close action for,
         * @param symbol - the symbol of the token to execute the close action for.
         *
         * @pre The pair of owner plus symbol has to exist otherwise no action is executed,
         * @pre If the pair of owner plus symbol exists, the balance has to be zero.
         */
        [[eosio::action]]
        void close( const name& owner, const symbol& symbol );

        /**
         * Get supply method.
         *
         * @details Gets the supply for token `sym_code`, created by `token_contract_account` account.
         *
         * @param token_contract_account - the account to get the supply for,
         * @param sym_code - the symbol to get the supply for.
         */
        static asset get_supply( const name& token_contract_account, const symbol_code& sym_code )
        {
            stats statstable( token_contract_account, sym_code.raw() );
            const auto& st = statstable.get( sym_code.raw() );
            return st.supply;
        }

        /**
         * Get balance method.
         *
         * @details Get the balance for a token `sym_code` created by `token_contract_account` account,
         * for account `owner`.
         *
         * @param token_contract_account - the token creator account,
         * @param owner - the account for which the token balance is returned,
         * @param sym_code - the token for which the balance is returned.
         */
        static asset get_balance( const name& token_contract_account, const name& owner, const symbol_code& sym_code )
        {
            accounts accountstable( token_contract_account, owner.value );
            const auto& ac = accountstable.get( sym_code.raw() );
            return ac.balance;
        }

        ACTION setconfig (  const uint32_t  earnings_rate,
                        // CDT BUG: /usr/local/eosio.cdt/bin/wasm-ld: error: CMakeFiles/bcdetoken.wasm.dir/bcdetoken.cpp.o: undefined symbol: __floatuntisf
                        // Todd Fleming: Several infrequently-used float-related intrinsics are currently missing. You got unlucky.
                        //const float       earnings_rate,
                        const uint8_t   equity_symbol_precision,
                        const string    equity_symbol,
                        const uint8_t   reward_symbol_precision,
                        const string    reward_symbol,
                        const name      rewards_contract);
  
        ACTION setearnrate (const uint32_t&  earnings_rate);

        ACTION update (const name&    account);

        ACTION withrewards (const name&    account);

        [[eosio::on_notify("eosio.token::transfer")]] void receive(const name &from, const name &to, const asset &quantity, const string &memo);

        using create_action = eosio::action_wrapper<"create"_n, &token::create>;
        using issue_action = eosio::action_wrapper<"issue"_n, &token::issue>;
        using retire_action = eosio::action_wrapper<"retire"_n, &token::retire>;
        using transfer_action = eosio::action_wrapper<"transfer"_n, &token::transfer>;
        using open_action = eosio::action_wrapper<"open"_n, &token::open>;
        using close_action = eosio::action_wrapper<"close"_n, &token::close>;
    private:

        const uint128_t       SCALER = 1000000000000000000;

        struct [[eosio::table]] account {
            asset       balance;
            uint128_t   scaled_earnings_balance;
            uint128_t   scaled_earnings_credited;

            uint64_t primary_key()const { return balance.symbol.code().raw(); }
        };

        struct [[eosio::table]] currency_stats {
            asset    supply;
            asset    max_supply;
            name     issuer;

            uint64_t primary_key()const { return supply.symbol.code().raw(); }
        };

        struct [[eosio::table]] config {
            symbol          equity_symbol;
            name            rewards_contract;
            symbol          rewards_symbol;
            uint128_t       scaled_earnings_per_token;
            uint128_t       scaled_remainder;
            uint32_t        earnings_rate;
            // /usr/local/Cellar/eosio.cdt/1.6.2/opt/eosio.cdt/bin/wasm-ld: error: CMakeFiles/token.dir/token.cpp.obj: undefined symbol: __floatuntisf
            // Todd Fleming: Several infrequently-used float-related intrinsics are currently missing. You got unlucky.
            // using earnings_rate_float as a placeholder for future usage when core eosio bug is fixed
            float           earnings_rate_float;
        };

        typedef singleton<"configs"_n, config> config_table;
        typedef multi_index<"configs"_n, config> config_table_placeholder;

        typedef eosio::multi_index< "accounts"_n, account > accounts;
        typedef eosio::multi_index< "stat"_n, currency_stats > stats;

        void sub_balance( const name& owner, const asset& value );
        void add_balance( const name& owner, const asset& value, const name& ram_payer );

        void payreward(const name from,
                        const name to,
                        const asset token_amount,
                        const string memo)
        {
            if (from == to) return;

            config_table config (get_self(), get_self().value);
            auto c = config.get();

            print("---------- Transfer -----------\n");
            print("Token Contract   :", name{c.rewards_contract}, "\n");
            print("From             :", name{from}, "\n");
            print("To               :", name{to}, "\n");
            print("Amount           :", token_amount, "\n");
            print("Memo             :", memo, "\n");

            action(
                permission_level{from, "active"_n},
                c.rewards_contract, "transfer"_n,
                std::make_tuple(from, to, token_amount, memo))
                .send();

            print("---------- End Transfer -------\n");
        }
};
