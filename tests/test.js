/* eslint-disable prettier/prettier */
const assert = require('assert');
const eoslime = require('eoslime').init('local');
//const eoslime = require('./../../eoslime').init({ url: 'https://kylin.eoscanada.com', chainId: '5fff1dae8dc8e2fc4d5b23b2c7665c97f9e9d8edf2b6485a86ba311c25639191' });
//const kylinMainAccount = eoslime.Account.load('bitcashtreas', '5J387fk1pL6ePE4wreQTiTYr2Atv3xcB2KckDnbjbd1swgZqquD');
//eoslime.Provider.defaultAccount = kylinMainAccount;

const SWAP_WASM_PATH = '../swaps/swaps/swaps.wasm';
const SWAP_ABI_PATH = '../swaps/swaps/swaps.abi';
const EOSIOTOKEN_WASM_PATH = '../swaps/eosiotoken/eosiotoken.wasm';
const EOSIOTOKEN_ABI_PATH = '../swaps/eosiotoken/eosiotoken.abi';
const REVENUE_WASM_PATH = '../swaps/revenue/revenue.wasm';
const REVENUE_ABI_PATH = '../swaps/revenue/revenue.abi';
 

describe('Swap Testing', function () {

    // Increase mocha(testing framework) time, otherwise tests fails
    this.timeout(150000);

    let swapsContract, eosTokenContract, escrowContract, revenueContract;
    let swapsAccount, eosTokenAccount, escrowAccount, revenueAccount, referrerAccount, providerAccount;
    let buyer1, buyer2, buyer3, buyer4, buyer5;
    let seller1, seller2, seller3, seller4, seller5;
    let accounts;
    let config;
    
    // const TOTAL_SUPPLY = '1000000000.0000 SYS';
    // const HOLDER_SUPPLY = '100.0000 SYS';

    before(async () => {

        accounts = await eoslime.Account.createRandoms(20);
        swapsAccount        = accounts[0];
        eosTokenAccount     = accounts[1];
        
        buyer1               = accounts[4];
        buyer2               = accounts[5];
        buyer3               = accounts[6];
        buyer4               = accounts[7];
        buyer5               = accounts[8];
        seller1               = accounts[9];
        seller2               = accounts[10];
        seller3               = accounts[11];
        seller4             = accounts[12];
        seller5             = accounts[13];
        escrowAccount       = accounts[14];
        revenueAccount       = accounts[15];
        referrerAccount     = accounts[16];
        providerAccount     = accounts[17];

        console.log (" Swap Account        : ", swapsAccount.name);
        console.log (" Escrow Account       : ", escrowAccount.name);
        console.log (" Revenue Account      : ", revenueAccount.name)
        console.log (" Referrer Account     : ", referrerAccount.name);
        console.log (" EOS Token            : ", eosTokenAccount.name)
        console.log (" buyer1               : ", buyer1.name);
        console.log (" buyer2               : ", buyer2.name);
        console.log (" buyer3               : ", buyer3.name);
        console.log (" seller1              : ", seller1.name);
        console.log (" seller2              : ", seller2.name);
        console.log (" seller3              : ", seller3.name);

        const initialBalance = '1000.0000 EOS'

        await swapsAccount.addPermission('eosio.code');
        await revenueAccount.addPermission('eosio.code');

        revenueContract = await eoslime.AccountDeployer.deploy (REVENUE_WASM_PATH, REVENUE_ABI_PATH, revenueAccount);
        swapsContract = await eoslime.AccountDeployer.deploy (SWAP_WASM_PATH, SWAP_ABI_PATH, swapsAccount);
        eosTokenContract = await eoslime.AccountDeployer.deploy (EOSIOTOKEN_WASM_PATH, EOSIOTOKEN_ABI_PATH, eosTokenAccount);
    
        await swapsContract.setconfig(escrowAccount.name, revenueAccount.name, 0.015000000000, "2.25 USD", 0.01500000000, 
            "0.5000 EOS", "1.0000 EOS", "10000.0000 EOS", "1000.0000 EOS", eosTokenAccount.name, { from: swapsAccount});

        await eosTokenContract.create(eosTokenContract.name, '100000.0000 EOS');

        console.log ("\n\n");
        console.log (" Buyers and sellers each receive 1000.0000 EOS at the beginning of all tests.")
        await eosTokenContract.issue(buyer1.name, initialBalance, 'memo', { from: eosTokenContract});
        await eosTokenContract.issue(buyer2.name, initialBalance, 'memo', { from: eosTokenContract});
        await eosTokenContract.issue(buyer3.name, initialBalance, 'memo', { from: eosTokenContract});
        await eosTokenContract.issue(buyer4.name, initialBalance, 'memo', { from: eosTokenContract});
        await eosTokenContract.issue(buyer5.name, initialBalance, 'memo', { from: eosTokenContract});

        await eosTokenContract.issue(seller1.name, initialBalance, 'memo', { from: eosTokenContract});
        await eosTokenContract.issue(seller2.name, initialBalance, 'memo', { from: eosTokenContract});
        await eosTokenContract.issue(seller3.name, initialBalance, 'memo', { from: eosTokenContract});
        await eosTokenContract.issue(seller4.name, initialBalance, 'memo', { from: eosTokenContract});
        await eosTokenContract.issue(seller5.name, initialBalance, 'memo', { from: eosTokenContract});

       // await eosTokenContract.issue(providerAccount.name, initialBalance, 'memo', { from: eosTokenContract});

        console.log (" The revenue contract must be configured with the provider (Swap) and the receiver (Referrer).")
        console.log (" The percentage the referrer receives of all fees must be configured in the trustline.")
        console.log (" Provider: ", swapsAccount.name)
        console.log (" Receiver: ", referrerAccount.name)
        console.log (" Percentage referral commission:  25% ")
        await revenueContract.addprovider (swapsAccount.name, "provider")
        await revenueContract.addreceiver (referrerAccount.name, "referrer")
        await revenueContract.addtrustline (swapsAccount.name, referrerAccount.name, 0.250000000, 'notes')
        console.log (" \n\n");

        console.log (" Here are the platform configurations.")
        let configTable = await swapsContract.provider.eos.getTableRows({
            code: swapsContract.name,
            scope: swapsContract.name,
            table: 'configs',
            json: true
        });
        config = configTable.rows[0];
        console.log (config)
        
        let trustlines = await revenueContract.provider.eos.getTableRows({
            code: revenueContract.name,
            scope: swapsAccount.name,
            table: 'trustlines',
            json: true
        });
        const trustline = trustlines.rows[0];
        console.log (" Here is the configured trustline for the referrer's commission")
        console.log (trustline)
        

    });

    beforeEach(async () => {
   
    });

    it('Should create a simple sell order', async () => {
        
        console.log (" Seller ", seller1.name, " transfers 10.0000 EOS to Escrow account: ", swapsAccount.name)
        await eosTokenContract.transfer (seller1.name, swapsAccount.name, '10.0000 EOS', "memo", { from: seller1 });

        console.log (" Seller ", seller1.name, " creates a SELL order for 10.0000 EOS @ $5 / EOS, using a referral from: ", referrerAccount.name)
        await swapsContract.create(seller1.name, 2, "10.0000 EOS", "10.0000 EOS", "EXACT_PRICE", "5.00 USD", 
            0.000000000, ["VENMO", "PAYPAL"], referrerAccount.name, {from: seller1 });
        
        console.log ("\nHere is the order that was created.....................");
        let tableResults = await swapsContract.provider.eos.getTableRows({
            code: swapsContract.name,
            scope: swapsContract.name,
            table: 'orders',
            json: true
        });
        console.log (tableResults)
        console.log ( "............................................................")

        const order_num = tableResults.rows[0].order_key

        console.log ("\nBuyer : ", buyer1.name, " accepts order : ", order_num, " and transfers fiat using Venmo.\n")
        await swapsContract.accept (order_num, buyer1.name, "VENMO", "10.0000 EOS", {from: buyer1})

        console.log (" Seller : ", seller1.name, " receives Fiat and approves the EOS funds to be released")
        await swapsContract.approvepay (order_num, seller1.name, {from: seller1})

        console.log (" Referrer : ", referrerAccount.name, " funds are held in the revenue contract until released.  ")
        console.log (" The referrer can release the funds by calling paypartner action.\n")
        await revenueContract.paypartner (referrerAccount.name, "0.0000 EOS");

        const balances = await eosTokenContract.provider.eos.getTableRows({
            code: eosTokenContract.name,
            scope: buyer1.name,
            table: 'accounts',
            json: true
        });
        const buyerBalance = balances.rows[0].balance

        const sellerBalances = await eosTokenContract.provider.eos.getTableRows({
            code: eosTokenContract.name,
            scope: seller1.name,
            table: 'accounts',
            json: true
        });
        const sellerBalance = sellerBalances.rows[0].balance

        const revenueTable = await eosTokenContract.provider.eos.getTableRows({
            code: eosTokenContract.name,
            scope: revenueAccount.name,
            table: 'accounts',
            json: true
        });
        const swapsBalance = revenueTable.rows[0].balance


        const referrerBalances = await eosTokenContract.provider.eos.getTableRows({
            code: eosTokenContract.name,
            scope: referrerAccount.name,
            table: 'accounts',
            json: true
        });
        const referrerBalance = referrerBalances.rows[0].balance

        console.log (' 4 Users balances have been adjusted based on the activity:');
        console.log (' Buyer New Balance: ', buyerBalance)
        console.log (' Seller New Balance: ', sellerBalance)
        console.log (' Swap New Balance: ', swapsBalance)
        console.log (' Referrer New Balance: ', referrerBalance)


        // buyer has original issue of 1000 + 10 they bought - 0.8488 in fees = 1009.1512 EOS
        assert.equal ('1009.1512 EOS', buyerBalance) 

        // seller should have original issue of 1000 - 10 they sold = 990.0000 EOS
        assert.equal ('990.0000 EOS', sellerBalance) 

        // swaps has 0.8488 in fees paid - referrer fee of 0.2122 = 0.6366 EOS
        assert.equal ('0.6366 EOS', swapsBalance) 

        // referrer has 25% (referrer fee) * total fee of 0.8488 = 0.2122 EOS
        assert.equal ('0.2122 EOS', referrerBalance) 


        console.log ("\n Here is the final state of the order; it may be erased........");
        tableResults = await swapsContract.provider.eos.getTableRows({
            code: swapsContract.name,
            scope: swapsContract.name,
            table: 'orders',
            json: true
        });
        console.log (tableResults)
        console.log ( "............................................................")

    });
    
    it ('Create a partial order', async () => {

        console.log (" Seller ", seller2.name, " transfers 100.0000 EOS to Escrow account: ", swapsAccount.name)
        await eosTokenContract.transfer (seller2.name, swapsAccount.name, '100.0000 EOS', "memo", { from: seller2 });

        console.log (" Seller ", seller2.name, " creates a SELL order for 100.0000 EOS @ $5 / EOS, using a referral from: ", referrerAccount.name)
        await swapsContract.create(seller2.name, 2, "100.0000 EOS", "4.0000 EOS", "EXACT_PRICE", "5.00 USD", 
            0.000000000, ["VENMO", "PAYPAL"], swapsAccount.name, {from: seller2 });
        
        console.log ("\nHere is the order that was created.....................");
        let tableResults = await swapsContract.provider.eos.getTableRows({
            code: swapsContract.name,
            scope: swapsContract.name,
            table: 'orders',
            json: true
        });
        console.log (tableResults)
        const priorLength = tableResults.rows.length;
        console.log ( "............................................................")

        const order_num = tableResults.rows[priorLength - 1].order_key

        console.log ("\nBuyer : ", buyer2.name, " accepts order : ", order_num, " and transfers fiat using Venmo.\n")
        await swapsContract.accept (order_num, buyer2.name, "VENMO", "4.0000 EOS", {from: buyer2})

        tableResults = await swapsContract.provider.eos.getTableRows({
            code: swapsContract.name,
            scope: swapsContract.name,
            table: 'orders',
            json: true
        });
        console.log (tableResults)
        assert.equal (tableResults.rows.length, priorLength + 1);
        console.log ( "............................................................")

    });

});