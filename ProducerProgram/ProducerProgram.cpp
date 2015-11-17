// ProducerProgram.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "StdInputHandler.h"
#include "CommandQueue.h"
#include <iostream>
#include <ace/Reactor.h>
#include "CommandEventHandler.h"
#include <thread>

int main(int argc, char* argv[])
{
    // Run the reactor in background thread.
    std::thread t([]{ 
        // Configure a command event handler
        ACE_Reactor::instance()->schedule_timer(
            new CommandEventHandler,
            nullptr,
            ACE_Time_Value(1, 0),
            ACE_Time_Value(1, 0)
        );
        ACE_Reactor::instance()->run_event_loop(); 
    });

    while (true) {
        try {
            std::string cmd_str = StdInputHandler::instance()->GetLine();
            std::shared_ptr<Command> command = StdInputHandler::instance()->MakeCommand(cmd_str);
            if (command != nullptr)
                CommandQueue::instance()->push(command);  // Queue up the command so that ACE_Reactor
                                                          // handler can handle it.
        }
        catch (std::runtime_error& e) {
            std::cerr << e.what() << std::endl;
        }
    }
    
	return 0;
}

