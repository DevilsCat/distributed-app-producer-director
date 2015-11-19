// ProducerProgram.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "StdInputHandler.h"
#include "CommandQueue.h"
#include <iostream>
#include <ace/Reactor.h>
#include "CommandEventHandler.h"
#include <thread>
#include <ace/Asynch_Acceptor.h>
#include <ace/Proactor.h>
#include "DirectorAsynchAcceptor.h"

int main(int argc, char* argv[])
{
    // Run the reactor in background thread.
    std::thread reactor_td([]{ 
        // Configure a command event handler
        ACE_Reactor::instance()->schedule_timer(
            new CommandEventHandler,
            nullptr,
            ACE_Time_Value(1, 0),
            ACE_Time_Value(1, 0)
        );
        ACE_Reactor::instance()->run_event_loop(); 
    });

    //Config the aio_acceptor
    ACE_INET_Addr addr(1234, ACE_LOCALHOST);
    DirectorAsynchAcceptor aio_acceptor;
    if (aio_acceptor.open(addr) != 0) {
        ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("%p\n"),
            ACE_TEXT("acceptor open")), 1);
    }

    //Run the proactor in proactor thread.
    std::thread proactor_td([]{
        ACE_Proactor::instance()->run_event_loop();
    });

    // Run UI event in main (UI) thread.
    while (true) {
        try {
            std::string cmd_str = StdInputHandler::instance()->GetLine();
            std::shared_ptr<Command> command = StdInputHandler::instance()->MakeCommand(cmd_str);
            if (command != nullptr)
                CommandQueue::instance()->push(command);  // Queue up the command so that ACE_Reactor
                                                          // handler can handle it.
            if (command && command->cmd_type == Command::kQuit)
                break;
        }
        catch (std::runtime_error& e) {
            std::cerr << e.what() << std::endl;
        }
    }

    reactor_td.join();
    proactor_td.join();
    
	return 0;
}

