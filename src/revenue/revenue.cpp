#include <revenue.hpp>

// workaround for bug on wildcard notification
// https://github.com/EOSIO/eosio.cdt/issues/497#issuecomment-484691582
ACTION revenue::dummy() {}

ACTION revenue::reset()
{
    require_auth(get_self());

    // config_table config_s(get_self(), get_self().value);
    // config_s.remove();

    receiver_table r_t(get_self(), get_self().value);
    auto r_itr = r_t.begin();
    while (r_itr != r_t.end())
    {
        r_itr = r_t.erase(r_itr);
    }

    provider_table p_t(get_self(), get_self().value);
    auto p_itr = p_t.begin();
    while (p_itr != p_t.end())
    {
        trustline_table t_t (get_self(), p_itr->provider_account.value);
        auto t_itr = t_t.begin ();
        while (t_itr != t_t.end())
        {
            // trstlinasset_table tt_t(get_self(), total_fee.symbol.code().raw());
            // auto tt_itr = tt_t.find(t_itr->trustline_id); 
            // tt_t.erase(tt_itr);
            t_itr = t_t.erase (t_itr);
        }
        p_itr = p_t.erase(p_itr);
    }

    blacklist_table bl_t (get_self(), get_self().value);
    auto bl_itr = bl_t.begin ();
    while (bl_itr != bl_t.end()) {
        bl_itr = bl_t.erase (bl_itr);
    }

    receipt_table rpt_t (get_self(), get_self().value);
    auto rct_itr = rpt_t.begin ();
    while (rct_itr != rpt_t.end()) {
        rct_itr = rpt_t.erase (rct_itr);
    }
}

// ACTION revenue::setconfig(const float &default_app_fee_share)
// {
//     //const vector<asset> whitelist_assets) {
//     require_auth(get_self());

//     config_table config_s(get_self(), get_self().value);
//     Config c = config_s.get_or_create(get_self(), Config());
//     c.default_app_fee_share = default_app_fee_share;
//     //c.whitelist_assets = whitelist_assets;
//     config_s.set(c, get_self());
// }

ACTION revenue::addreceiver(const name &receiver_account, const string& description)
{
    require_auth(get_self());
    receiver_table r_t(get_self(), get_self().value);

    check(is_account(receiver_account), "Receiver account is not an EOS account: " + receiver_account.to_string());
    check(r_t.find(receiver_account.value) == r_t.end(), "Account is already configured as a receiver account: " + receiver_account.to_string());

    r_t.emplace(get_self(), [&](auto& r) {
        r.receiver_account = receiver_account;
        r.description = description;
    });
}

ACTION revenue::remreceiver(const name &receiver_account)
{
    require_auth(get_self());
    receiver_table r_t(get_self(), get_self().value);

    check(is_account(receiver_account), "Receiver account is not an EOS account: " + receiver_account.to_string());
    check(r_t.find(receiver_account.value) == r_t.end(), "Account is not an existing receiver account: " + receiver_account.to_string());

    r_t.erase(r_t.find(receiver_account.value));
}

ACTION revenue::addprovider(const name &provider_account, const string &description)
{
    require_auth(get_self());
    provider_table p_t(get_self(), get_self().value);

    check(is_account(provider_account), "provider account is not an EOS account: " + provider_account.to_string());
    check(p_t.find(provider_account.value) == p_t.end(), "Account is already configured as a provider account: " + provider_account.to_string());

    p_t.emplace(get_self(), [&](auto& p) {
        p.provider_account = provider_account;
        p.description = description;
    });
}

ACTION revenue::remprovider(const name &provider_account)
{
    require_auth(get_self());
    provider_table p_t(get_self(), get_self().value);

    check(is_account(provider_account), "provider account is not an EOS account: " + provider_account.to_string());
    check(p_t.find(provider_account.value) == p_t.end(), "Account is not an existing provider account: " + provider_account.to_string());

    p_t.erase(p_t.find(provider_account.value));
}

ACTION revenue::addtrustline(const name &provider_account, const name &receiver_account, const float &rev_share, const string &notes)
{
    require_auth(get_self());
    check(is_account(provider_account), "Provider account is not a valid EOS account.");
    check(is_account(receiver_account), "Receiver account is not a valid EOS account.");

    trustline_table t_t(get_self(), provider_account.value);
    provider_table p_t(get_self(), get_self().value);
    check(p_t.find(provider_account.value) != p_t.end(), "Provider account is not an existing provider account: " + provider_account.to_string());

    receiver_table r_t(get_self(), get_self().value);
    check(r_t.find(receiver_account.value) != r_t.end(), "Reciever account is not an existing receiver account: " + receiver_account.to_string());

    check(rev_share > 0 && rev_share <= 1, "Fee share must be greater than zero and less than or equal to one: " + std::to_string(rev_share));

    t_t.emplace (get_self(), [&](auto& t) {
        t.trustline_id = t_t.available_primary_key();
        t.receiver_account = receiver_account;
        t.rev_share = rev_share;
        t.notes = notes;
    });
}

ACTION revenue::updtrustline(const name &provider_account, const name &receiver_account, const float &rev_share, const string &notes)
{
    require_auth(get_self());
    trustline_table t_t(get_self(), provider_account.value);
    auto receiver_index = t_t.get_index<"byreceiver"_n>();
    auto receiver_itr = receiver_index.find(receiver_account.value);

    check(receiver_itr != receiver_index.end(), "Trustline with provider and receiver not found. Provider: " + provider_account.to_string() + "; Receiver: " + receiver_account.to_string());

    check(rev_share > 0 && rev_share <= 1, "Revenue share must be greater than zero and less than or equal to one: " + std::to_string(rev_share));

    receiver_index.modify(receiver_itr, get_self(), [&](auto& t) {
        t.rev_share = rev_share;
        t.notes = notes;
        t.updated_date = current_block_time().to_time_point().sec_since_epoch();
    });
}

ACTION revenue::remtrustline(const name& receiver_account, const name& provider_account)
{
    require_auth(get_self());
    trustline_table t_t(get_self(), provider_account.value);
    auto receiver_index = t_t.get_index<"byreceiver"_n>();
    auto receiver_itr = receiver_index.find(receiver_account.value);

    check(receiver_itr != receiver_index.end(), "Trustline with provider and receiver not found. Provider: " + provider_account.to_string() + "; Receiver: " + receiver_account.to_string());

    receiver_index.erase(receiver_itr);
}

ACTION revenue::paypartner (const name& receiver_account, 
                             const asset& symbol_to_pay ) {

    check ( has_auth (receiver_account) || has_auth (get_self()), "Must be approved by contract account or receiver. Parnter Account: " \
            + receiver_account.to_string() + "  Contract/Self : " + get_self().to_string());

    balances_due_table bd_t (get_self(), receiver_account.value);
    auto bd_itr = bd_t.find (symbol_to_pay.symbol.code().raw());
    check (bd_itr != bd_t.end(), "Partner account does not have a balance due for this symbol. Partner Account: " \
            + receiver_account.to_string() + "  Symbol: " + symbol_to_pay.to_string());

    check (bd_itr->amount_due.amount >= 0, "Partner account and asset found, but nothing is due. Partner Account: " \
            + receiver_account.to_string() + "  Symbol: " + symbol_to_pay.to_string());

    //if (bd_itr == bd_t.end() || bd_itr->amount_due.amount <= 0) return;  // nothing to pay

    paytoken (bd_itr->token_contract, get_self(), receiver_account, bd_itr->amount_due, bd_itr->memo);

    bd_t.modify (bd_itr, get_self(), [&](auto& bd) {
        bd.amount_due       *= 0;
        bd.last_paid_date   = current_block_time().to_time_point().sec_since_epoch();
        bd.memo             = string {""};
    });
}

ACTION revenue::paytrustline(const name &provider_account, 
                            const name &receiver_account, 
                            const name& token_contract, 
                            const asset &total_fee, 
                            const string& memo)
{
    require_auth(provider_account);

    print (" Paying to trustline - Total Fee    : ", total_fee, "\n");

    provider_table p_t(get_self(), get_self().value);
    check(is_account(provider_account), "provider account is not an EOS account: " + provider_account.to_string());
    check(p_t.find(provider_account.value) != p_t.end(), "Account is not an existing provider account: " + provider_account.to_string());

    receiver_table r_t(get_self(), get_self().value);
    check(is_account(receiver_account), "Receiver account is not an EOS account: " + receiver_account.to_string());
    check(r_t.find(receiver_account.value) != r_t.end(), "Account is not an existing receiver account: " + receiver_account.to_string());

    blacklist_table bl_t(get_self(), get_self().value);
    if (bl_t.find(provider_account.value) != bl_t.end())
        return; // account is on the blacklist
    if (bl_t.find(receiver_account.value) != bl_t.end())
        return; // account is on the blacklist

    trustline_table t_t(get_self(), provider_account.value);
    auto receiver_index = t_t.get_index<"byreceiver"_n>();
    auto receiver_itr = receiver_index.find(receiver_account.value);
    if (receiver_itr == receiver_index.end())
        return; // no trustline found; do nothing

    asset partner_payment = adjust_asset(total_fee, receiver_itr->rev_share);

    //string catmemo{"Referral payment from revenue order. " + memo};
    //paytoken(token_contract, get_self(), receiver_itr->receiver_account, partner_payment, catmemo);
    add_to_balance (provider_account, receiver_itr->receiver_account, token_contract, partner_payment, memo);

    receiver_index.modify(receiver_itr, provider_account, [&](auto &t) {
        t.trx_count++;
        t.updated_date = current_block_time().to_time_point().sec_since_epoch();
    });

    print (" PAY Symbol   : ", total_fee.symbol, "\n");
    print (" PAY Symbol Code   : ", total_fee.symbol.code(), "\n");
    print (" PAY Symbol code raw    : ", total_fee.symbol.code().raw(), "\n");

    print (" Total fee symbol code RAW - scope: ", std::to_string(total_fee.symbol.code().raw()), "\n");
    trstlinasset_table tt_t(get_self(), total_fee.symbol.code().raw());
    auto tt_index = tt_t.get_index<"bytrstlineid"_n>();
    auto tt_itr = tt_index.find(receiver_itr->trustline_id);
    if (tt_itr == tt_index.end()) {
        tt_t.emplace(provider_account, [&](auto& tt) {
            tt.trustline_asset_id = tt_t.available_primary_key();
            tt.amount_in = total_fee;
            tt.amount_out = partner_payment;
            tt.token_contract = token_contract;
            print (" Total Fee : ", total_fee, "\n");
        });
    } else {
        check (tt_itr->token_contract == token_contract, "The token contract does not match the asset.");
        tt_index.modify (tt_itr, provider_account, [&](auto &tt) {
            tt.amount_in += total_fee;
            tt.amount_out += partner_payment;
        });
    }


}

ACTION revenue::blacklist(const name &account)
{
    require_auth(get_self());
    blacklist_table bl_t(get_self(), get_self().value);

    check(is_account(account), "Account is not an EOS account: " + account.to_string());
    check(bl_t.find(account.value) == bl_t.end(), "Account is already on the blacklist:  " + account.to_string());

    bl_t.emplace(get_self(), [&](auto& b) {
        b.account = account;
    });
}

ACTION revenue::whitelist(const name &account)
{
    require_auth(get_self());
    blacklist_table bl_t(get_self(), get_self().value);

    check(is_account(account), "Account is not an EOS account: " + account.to_string());
    check(bl_t.find(account.value) != bl_t.end(), "Account is not on the blacklist: " + account.to_string());

    bl_t.erase(bl_t.find (account.value));
}

void revenue::receive(const name &from, const name &to, const asset &quantity, const string &memo)
{
    if (to != get_self())
    {
        return;
    } // not sending to rev account

    receipt_table r_t(get_self(), get_self().value);
    r_t.emplace(get_self(), [&](auto &r) {
        r.receipt_id = r_t.available_primary_key();
        r.revenue_from = from;
        r.token_contract = get_first_receiver();
        r.revenue_amount = quantity;
        r.memo = memo;
    });
}
