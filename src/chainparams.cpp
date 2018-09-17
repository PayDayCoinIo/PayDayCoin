// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "assert.h"

#include "chainparams.h"
#include "main.h"
#include "util.h"
#include "base58.h"

#include <boost/assign/list_of.hpp>

using namespace boost::assign;

struct SeedSpec6 {
    uint8_t addr[16];
    uint16_t port;
};

#include "chainparamsseeds.h"

//
// Main network
//

// Convert the pnSeeds array into usable address objects.
static void convertSeeds(std::vector<CAddress> &vSeedsOut, const unsigned int *data, unsigned int count, int port)
{
     // It'll only connect to one or two seed nodes because once it connects,
     // it'll get a pile of addresses with newer timestamps.
     // Seed nodes are given a random 'last seen time' of between one and two
     // weeks ago.
     const int64_t nOneWeek = 7*24*60*60;
    for (unsigned int k = 0; k < count; ++k)
    {
        struct in_addr ip;
        unsigned int i = data[k], t;
        
        // -- convert to big endian
        t =   (i & 0x000000ff) << 24u
            | (i & 0x0000ff00) << 8u
            | (i & 0x00ff0000) >> 8u
            | (i & 0xff000000) >> 24u;
        
        memcpy(&ip, &t, sizeof(ip));
        
        CAddress addr(CService(ip, port));
        addr.nTime = GetTime()-GetRand(nOneWeek)-nOneWeek;
        vSeedsOut.push_back(addr);
    }
}

class CMainParams : public CChainParams {
public:
    CMainParams() {
        // The message start string is designed to be unlikely to occur in normal data.
        // The characters are rarely used upper ASCII, not valid as UTF-8, and produce
        // a large 4-byte int at any alignment.
        pchMessageStart[0] = 0xcf;
        pchMessageStart[1] = 0x2a;
        pchMessageStart[2] = 0xd3;
        pchMessageStart[3] = 0x8e;
        vAlertPubKey = ParseHex("04e317aba52be44f4cb4fb5a030fa014328cc85415dcdd1172cfaca76af8dd39f58c99b2ff743fd742cdaa2641d6817aa861e60bf751c029c2703f08dc726f1cd2");
        nDefaultPort = 7214;
        nRPCPort = 7215;
        bnProofOfWorkLimit = CBigNum(~uint256(0) >> 16);

        // Build the genesis block.
            // transaction:  Coinbase(hash=3c8eb5ab2277426f2f48bd70eeb32416b1c94456a3e56e1871d15b568dde3e1e, nTime=1518409800, ver=1, vin.size=1, vout.size=1, nLockTime=0)
            // CTxIn(COutPoint(0000000000, 4294967295), coinbase 00012a284f63742032322c20323031373a20426974636f696e2070726963652061626f76652024362c303030)
            // CTxOut(empty)

        const char* pszTimestamp = "Start PayDay Coin at Thursday, 12-Apr-18 11:05:00 UTC";
        std::vector<CTxIn> vin;
        vin.resize(1);
        vin[0].scriptSig = CScript() << 0 << CBigNum(42) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        std::vector<CTxOut> vout;
        vout.resize(1);
        vout[0].SetEmpty();
        CTransaction txNew(1, 1523531100, vin, vout, 0);

		// commented for resolve issue: when start with -datadir arg, debug.log file writed to default folder
        //LogPrintf("Genesis mainnet Transaction:  %s\n", txNew.ToString().c_str());
		//
		
        genesis.vtx.push_back(txNew);

        genesis.hashPrevBlock = 0;
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 1;
        genesis.nTime    = 1523531100;
        genesis.nBits = 504365040; // bnProofOfWorkLimit.GetCompact();
        genesis.nNonce   = 10285;

        hashGenesisBlock = genesis.GetHash();

         if (hashGenesisBlock != uint256("0xfbf8f2c6c429973be775cac61bcb7eb3dac0d95d134a63d91d505608f3febadc") ) {
            LogPrintf("================Genesis error===================\n");
            LogPrintf("Genesis hash: %s \n", genesis.GetHash().ToString().c_str());
            LogPrintf("Genesis nTime: %s \n", genesis.nTime);
            LogPrintf("Genesis nBits: %s \n", genesis.nBits);
            LogPrintf("Genesis nNonce: %s \n", genesis.nNonce);
            LogPrintf("Genesis Merkle: %s \n", genesis.hashMerkleRoot.ToString().c_str());
            LogPrintf("===================================\n");
        }

        assert(hashGenesisBlock == uint256("0xfbf8f2c6c429973be775cac61bcb7eb3dac0d95d134a63d91d505608f3febadc"));
        assert(genesis.hashMerkleRoot == uint256("0x306e9995cae4eaadbb512464c8c1b3c7195bf84bc88caf3c4282fd1d49b67d3d"));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,50);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,85);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,83);
        base58Prefixes[STEALTH_ADDRESS] = std::vector<unsigned char>(1,43);
        base58Prefixes[EXT_PUBLIC_KEY] = list_of(0x04)(0x88)(0xB2)(0x1E).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = list_of(0x04)(0x88)(0xAD)(0xE4).convert_to_container<std::vector<unsigned char> >();

		vSeeds.push_back(CDNSSeedData("a",  "108.61.190.194"));
		vSeeds.push_back(CDNSSeedData("b",  "209.250.250.22"));
		vSeeds.push_back(CDNSSeedData("c",  "45.77.230.162"));
		vSeeds.push_back(CDNSSeedData("d",  "199.247.18.81"));
		vSeeds.push_back(CDNSSeedData("e",  "199.247.16.22"));
		vSeeds.push_back(CDNSSeedData("f",  "199.247.22.240"));

        vSeeds.push_back(CDNSSeedData("a.paydaycoin.io",  "a.paydaycoin.io"));
        vSeeds.push_back(CDNSSeedData("b.paydaycoin.io",  "b.paydaycoin.io"));
        vSeeds.push_back(CDNSSeedData("c.paydaycoin.io",  "c.paydaycoin.io"));
        vSeeds.push_back(CDNSSeedData("d.paydaycoin.io",  "d.paydaycoin.io"));
        vSeeds.push_back(CDNSSeedData("e.paydaycoin.io",  "e.paydaycoin.io"));
        vSeeds.push_back(CDNSSeedData("f.paydaycoin.io",  "f.paydaycoin.io"));
        vSeeds.push_back(CDNSSeedData("g.paydaycoin.io",  "g.paydaycoin.io"));
        vSeeds.push_back(CDNSSeedData("h.paydaycoin.io",  "h.paydaycoin.io"));
        vSeeds.push_back(CDNSSeedData("i.paydaycoin.io",  "i.paydaycoin.io"));
        vSeeds.push_back(CDNSSeedData("j.paydaycoin.io",  "j.paydaycoin.io"));

        convertSeeds(vFixedSeeds, pnSeed, ARRAYLEN(pnSeed), nDefaultPort);
        //vFixedSeeds.clear();
        //vSeeds.clear();

        //getHardcodedSeeds(vFixedSeeds);

        nPoolMaxTransactions = 3;
        //strSporkKey = "046f78dcf911fbd61910136f7f0f8d90578f68d0b3ac973b5040fb7afb501b5939f39b108b0569dca71488f5bbf498d92e4d1194f6f941307ffd95f75e76869f0e";
        //strMasternodePaymentsPubKey = "046f78dcf911fbd61910136f7f0f8d90578f68d0b3ac973b5040fb7afb501b5939f39b108b0569dca71488f5bbf498d92e4d1194f6f941307ffd95f75e76869f0e";
        strDarksendPoolDummyAddress = "MWc1TrChdsnY7bPJbQDeyhkyeC8YHmzrx1";

        nLastPOWBlock = 10000;
        nPOSStartBlock = 7000;
    }

    virtual const CBlock& GenesisBlock() const { return genesis; }
    virtual Network NetworkID() const { return CChainParams::MAIN; }

    virtual const vector<CAddress>& FixedSeeds() const {
        return vFixedSeeds;
    }
protected:
    CBlock genesis;
    vector<CAddress> vFixedSeeds;
};
static CMainParams mainParams;


//
// Testnet
//

class CTestNetParams : public CMainParams {
public:
    CTestNetParams() {
        // The message start string is designed to be unlikely to occur in normal data.
        // The characters are rarely used upper ASCII, not valid as UTF-8, and produce
        // a large 4-byte int at any alignment.
        pchMessageStart[0] = 0xfc;
        pchMessageStart[1] = 0xa2;
        pchMessageStart[2] = 0x3d;
        pchMessageStart[3] = 0xe8;
        bnProofOfWorkLimit = CBigNum(~uint256(0) >> 16);
        vAlertPubKey = ParseHex("04e317aba52be44f4cb4fb5a030fa014328cc85415dcdd1172cfaca76af8dd39f58c99b2ff743fd742cdaa2641d6817aa861e60bf751c029c2703f08dc726f1cd2");
        nDefaultPort = 7214;
        nRPCPort = 7215;
        strDataDir = "testnet";

        hashGenesisBlock = genesis.GetHash();
        assert(hashGenesisBlock == uint256("0xfbf8f2c6c429973be775cac61bcb7eb3dac0d95d134a63d91d505608f3febadc"));

        vFixedSeeds.clear();
        vSeeds.clear();

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,73);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY]     = std::vector<unsigned char>(1,83);
        base58Prefixes[STEALTH_ADDRESS] = std::vector<unsigned char>(1,40);
        base58Prefixes[EXT_PUBLIC_KEY] = list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();

        convertSeeds(vFixedSeeds, pnTestnetSeed, ARRAYLEN(pnTestnetSeed), nDefaultPort);

        nLastPOWBlock = 0x7fffffff;
    }
    virtual Network NetworkID() const { return CChainParams::TESTNET; }
};
static CTestNetParams testNetParams;


static CChainParams *pCurrentParams = &mainParams;

const CChainParams &Params() {
    return *pCurrentParams;
}

void SelectParams(CChainParams::Network network) {
    switch (network) {
        case CChainParams::MAIN:
            pCurrentParams = &mainParams;
            break;
        case CChainParams::TESTNET:
            pCurrentParams = &testNetParams;
            break;
        default:
            assert(false && "Unimplemented network");
            return;
    }
}

bool SelectParamsFromCommandLine() {
    
    bool fTestNet = GetBoolArg("-testnet", false);
    
    if (fTestNet) {
        SelectParams(CChainParams::TESTNET);
    } else {
        SelectParams(CChainParams::MAIN);
    }
    return true;
}
