// DirectorProgram.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Director.h"
#include "utils.h"
#include <iostream>
#include <ace/Reactor.h>
#include "RdWrSockSvcHandler.h"
#include <ace/Connector.h>
#include <ace/SOCK_Connector.h>

#define MIN_ARG_NUM                 5
#define PROGRAM_POS                 0
#define PORT_NUM_POS                1
#define IP_ADDR_POS                 2
#define MIN_THREAD_POS              3
#define FIRST_SCRIPT_FILE_POS       4
#define UNINITIALIZED_PLAYER_NUM    -1

void PrintUsage(char** argv) {
    std::cout << "usage: " << argv[PROGRAM_POS] << "<port> <ip_address> <min_threads> <script_file>+" << std::endl;
}

int main(int argc, char* argv[])
{
    // arguments validation
    if (argc < MIN_ARG_NUM ||
            !is_number(argv[PORT_NUM_POS]) ||
            !is_ip_addr(argv[IP_ADDR_POS]) ||
            !is_number(argv[MIN_THREAD_POS])) {
        PrintUsage(argv);
        return EXIT_FAILURE;
    }

    // validate all possible filename position
    for (int i = FIRST_SCRIPT_FILE_POS; i < argc; ++i) {
        if (!is_filename(argv[i])) {
            PrintUsage(argv);
            return EXIT_FAILURE;
        }
    }

    // Get arguments
    int port_num = to_number(argv[PORT_NUM_POS]);
    std::string addr_str(argv[IP_ADDR_POS]);
    int min_num_threads = to_number(argv[MIN_THREAD_POS]);
    std::vector<std::string> script_filenames;
    for (int i = FIRST_SCRIPT_FILE_POS; i < argc; ++i)
        script_filenames.push_back(argv[i]);

    // Construct a Director pointer so that it can point to different Director's constructor
    Director *director = nullptr;
    try {
        director = new Director(script_filenames, min_num_threads);
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        if (director != nullptr) delete director;
        return EXIT_FAILURE;
    }

    // Do some other stuffs
    ACE_Reactor::instance()->schedule_timer (
        director,
        nullptr,
        ACE_Time_Value(),
        ACE_Time_Value(0, 10000)
    );

    ACE_INET_Addr addr(1234, ACE_LOCALHOST);

    ACE_Connector<RdWrSockSvcHandler, ACE_SOCK_Connector> connector;

    RdWrSockSvcHandler* svc_handler = new RdWrSockSvcHandler(director);
    director->SetSvcHandler(svc_handler);
    if (connector.connect(svc_handler, addr) == -1) {
        ACE_ERROR((LM_ERROR, "Connection Failed"));
    }

    ACE_Reactor::instance()->run_event_loop();

    return 0;
}