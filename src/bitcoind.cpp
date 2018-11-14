// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "rpcserver.h"
#include "rpcclient.h"
#include "init.h"
#include <boost/algorithm/string/predicate.hpp>

#include <iostream>

//#define BOOST_FILESYSTEM_VERSION 2
#include <boost/process.hpp>
#include <string>
#include <vector>

#include <boost/thread.hpp>

#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>

#include <boost/filesystem.hpp>

#define MTX_NAME    "pdd_onair_restart"

boost::filesystem::ofstream outfile;
namespace bp = ::boost::process;

int checkRestart();

void WaitForShutdown(boost::thread_group* threadGroup)
{

    bool fShutdown = ShutdownRequested();
    // Tell the main threads to shutdown.
    while (!fShutdown)
    {
        MilliSleep(200);
        fShutdown = ShutdownRequested();
    }
    if (threadGroup)
    {
        threadGroup->interrupt_all();
        threadGroup->join_all();
    }
}

int checkRestart()
{
        outfile << "checking restart " <<  bp::self::get_instance().get_id() << std::endl;
    int rv = 0;
    try
    {
                boost::interprocess::named_mutex g_mtx(boost::interprocess::open_only, MTX_NAME);
                outfile << "mutex opened seems i'm child" << std::endl;
                if( g_mtx.timed_lock(boost::get_system_time() + boost::posix_time::seconds{ 40 }))
                {
                        outfile << "mutex locked" <<std::endl;
                        g_mtx.unlock();
                        boost::interprocess::named_mutex::remove(MTX_NAME);
                        rv=0;
                } else rv=2;
    }
    catch (const boost::interprocess::interprocess_exception &ex)
    {
        outfile << "Lock Exception: " << ex.what() << " code " << ex.get_error_code() << std::endl;
        rv = 2;
        //mutex not found
        if (ex.get_error_code()==7)
                rv =1;
    }
        outfile << "rv = "<< rv << " pid " <<  bp::self::get_instance().get_id() << std::endl;
    return rv;
}

bool doRestart(int argc, char *argv[])
{
    if (argc == 0)
        return false;

    std::string selfPath(argv[0]);

    std::vector<std::string> selfArgs;
    for (int i = 0; i < argc; i++)
        selfArgs.push_back(argv[i]);

     boost::interprocess::named_mutex g_mtx(boost::interprocess::create_only, MTX_NAME);
     g_mtx.lock();
     outfile << "mutex locked" <<std::endl;

    bp::context ctx;

    bp::child chProc = bp::launch(selfPath, selfArgs, ctx);

    outfile << "Child is Running: " << chProc.get_id() << " from pid " <<  bp::self::get_instance().get_id() << std::endl;
    //boost::this_thread::sleep_for(boost::chrono::seconds{ 20 });
    MilliSleep(20000);
    outfile << "unlocking mutex" << std::endl;
    g_mtx.unlock();
    return true;
}

//////////////////////////////////////////////////////////////////////////////
//
// Start
//
bool AppInit(int argc, char* argv[])
{
    boost::thread_group threadGroup;

    bool fRet = false;
    fHaveGUI = false;
    try
    {
        //
        // Parameters
        //
        // If Qt is used, parameters/bitcoin.conf are parsed in qt/bitcoin.cpp's main()
        ParseParameters(argc, argv);
        if (!boost::filesystem::is_directory(GetDataDir(false)))
        {
            fprintf(stderr, "Error: Specified directory does not exist\n");
            Shutdown();
        }
        ReadConfigFile(mapArgs, mapMultiArgs);

        if (mapArgs.count("-?") || mapArgs.count("--help"))
        {
            // First part of help message is specific to bitcoind / RPC client
            std::string strUsage = _("PayDay version") + " " + FormatFullVersion() + "\n\n" +
                _("Usage:") + "\n" +
                  "  PayDayd [options]                     " + "\n" +
                  "  PayDayd [options] <command> [params]  " + _("Send command to -server or PayDayd") + "\n" +
                  "  PayDayd [options] help                " + _("List commands") + "\n" +
                  "  PayDayd [options] help <command>      " + _("Get help for a command") + "\n";

            strUsage += "\n" + HelpMessage();

            fprintf(stdout, "%s", strUsage.c_str());
            return false;
        }

        // Command-line RPC
        for (int i = 1; i < argc; i++)
            if (!IsSwitchChar(argv[i][0]) && !boost::algorithm::istarts_with(argv[i], "payday:"))
                fCommandLine = true;

        if (fCommandLine)
        {
            if (!SelectParamsFromCommandLine()) {
                fprintf(stderr, "Error: invalid combination of -regtest and -testnet.\n");
                return false;
            }
            int ret = CommandLineRPC(argc, argv);
            exit(ret);
        }


#if !WIN32

        fDaemon = GetBoolArg("-daemon", false);
        if (fDaemon)
        {
            // Daemonize
            pid_t pid = fork();
            if (pid < 0)
            {
                fprintf(stderr, "Error: fork() returned %d errno %d\n", pid, errno);
                return false;
            }
            if (pid > 0) // Parent process, pid is child process id
            {
                CreatePidFile(GetPidFile(), pid);
                return true;
            }
            // Child process falls through to rest of initialization

            pid_t sid = setsid();
            if (sid < 0)
                fprintf(stderr, "Error: setsid() returned %d errno %d\n", sid, errno);
        }
#endif

        fRet = AppInit2(threadGroup);

    }
    catch (std::exception& e) {
        PrintException(&e, "AppInit()");
    } catch (...) {
        PrintException(NULL, "AppInit()");
    }

    if (!fRet)
    {
        threadGroup.interrupt_all();
        // threadGroup.join_all(); was left out intentionally here, because we didn't re-test all of
        // the startup-failure cases to make sure they don't result in a hang due to some
        // thread-blocking-waiting-for-another-thread-during-startup case
    } else {
        WaitForShutdown(&threadGroup);
    }
    Shutdown();

    return fRet;
}

extern void noui_connect();

int main(int argc, char* argv[])
{
    bool fRet = false;

    bool rv = checkRestart();
    while (rv) {
        MilliSleep(500);
        rv = checkRestart();
    }

    // Connect bitcoind signal handlers
    noui_connect();

    fRet = AppInit(argc, argv);

    if (RestartRequested()) {
        try
        {
            char fn [256];
            //fn = "output_";
            sprintf (fn,"output_%d", bp::self::get_instance().get_id());
            outfile.open (fn);
            outfile << "hello";
            outfile << "i'm Process: " << bp::self::get_instance().get_id() << std::endl;

            bool rv;
            rv = checkRestart();
            outfile << "checkRestart: " << rv << std::endl;
            if (rv==1) {
                            rv = doRestart(argc, argv);
                            outfile << "do restart rv " << rv << std::endl;
    //	                boost::this_thread::sleep_for(boost::chrono::seconds{ 10 });

                    }
            else if (rv==0) {
                    // i'm child
                    outfile << "i'm child" << std::endl;
                    outfile << "doing child stuff for 20 seconds" << std::endl;
                    MilliSleep(20000);
                    //boost::this_thread::sleep_for(boost::chrono::seconds{ 20 });
                    outfile << "child stuff done" << std::endl;


            } else if (rv==2) {
                    outfile << "i'm child and have problems with mutex locking" << std::endl;
            }


        }
        catch (const boost::interprocess::interprocess_exception &ex)
        {
            outfile << "Lock Exception: " << ex.what() << std::endl;
        }
        catch (boost::filesystem::filesystem_error &ex)
        {
            outfile << "FS Exception: " << ex.what() << std::endl;
        }

        outfile << std::endl << "The End!!!" << std::endl << std::endl;

    }

    if (fRet && fDaemon)
        return 0;

    return (fRet ? 0 : 1);
}
