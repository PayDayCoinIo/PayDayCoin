// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef BITCOIN_INIT_H
#define BITCOIN_INIT_H

#include "wallet.h"

//#define BOOST_FILESYSTEM_VERSION 2
#include "boost/process.hpp"

#include <vector>

#include <boost/thread.hpp>
#include <boost/chrono.hpp>

#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

namespace bp = ::boost::process;
using namespace boost;

#define MTX_NAME    "pdd_onair_restart"

namespace boost {
    class thread_group;
} // namespace boost

extern CWallet* pwalletMain;
void StartShutdown();
void StartRestart();

bool ShutdownRequested();
bool RestartRequested();

void Shutdown();

bool ShutdownRequested();
bool RestartRequested();

int checkRestart();
bool doRestart(int argc, char *argv[]);

bool AppInit2(boost::thread_group& threadGroup);
std::string HelpMessage();
extern bool fOnlyTor;

#endif
