// ProducerProgram.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "StdInputHandler.h"
#include "CommandQueue.h"
#include "ReactorEventHandlers.h"
#include "ViewRenderer.h"
#include <thread>
#include <ace/Reactor.h>
#include <ace/Asynch_Acceptor.h>
#include <ace/Proactor.h>

#define NUM_ARGS            2
#define PROGRAM_ARG_POS     0
#define PORT_ARG_POS        1

#define ACE_TIMER_SECS      0
#define ACE_TIMER_MCROSECS  100000

#define DEFAULT_PORT        1234

int main(int argc, char* argv[])
{
    // Validate coming argument.
    if (argc > NUM_ARGS ||
        argc == NUM_ARGS && !is_number(argv[PORT_ARG_POS])) {
        std::cout << "usage: " << argv[PROGRAM_ARG_POS] << " [port]" << std::endl;
        return EXIT_FAILURE;
    }

    unsigned port_num;
    port_num = argc == NUM_ARGS ? to_number(argv[PORT_ARG_POS]) : DEFAULT_PORT;

    // Setup views
    ViewRenderer::instance()->AddView("Play", TableView<PlayTableViewCell>::MakeView("Play List"));
    ViewRenderer::instance()->AddView("Debug", TableView<DebugTableViewCell>::MakeView("Debug Messages"));
    ViewRenderer::instance()->Render(ViewRenderer::sAllViews);  // Render an empty view.

    //Config the ssynchrnous io acceptor
    ACE_INET_Addr addr(port_num, ACE_LOCALHOST);
    if (Producer::instance()->open(addr)) {
        ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("%p\n"),
            ACE_TEXT("acceptor open")), EXIT_FAILURE);
    }
    // Run the reactor in background thread.
    std::thread reactor_td([]{ 
        // Register a command event handler
        CommandEventHandler* cmd_event_handler;
        ACE_NEW_NORETURN(cmd_event_handler, CommandEventHandler);
        ACE_Reactor::instance()->schedule_timer(
            cmd_event_handler,
            nullptr,
            ACE_Time_Value(ACE_TIMER_SECS, ACE_TIMER_MCROSECS),
            ACE_Time_Value(ACE_TIMER_SECS, ACE_TIMER_MCROSECS)
        );

        // Register a signal handler to deal with Ctl-C
        SignalEventHandler* signal_event_handler;
        ACE_NEW_NORETURN(signal_event_handler, SignalEventHandler);
        ACE_Reactor::instance()->register_handler(SIGINT, signal_event_handler);

        ACE_Reactor::instance()->run_event_loop(); 
    });

    //Run the proactor in proactor thread.
    std::thread proactor_td([]{
        ACE_Proactor::instance()->run_event_loop();
    });

    // Run UI event in main (UI) thread.
    while (true) {
        try {
            std::string cmd_str = StdInputHandler::GetLine();
            std::shared_ptr<Command> command = StdInputHandler::instance()->MakeCommand(cmd_str);
            if (command != nullptr)                       // if command vaild, queue up the command 
                CommandQueue::instance()->push(command);  // so that ACE_Reactor handler can handle it.
            if (command && command->cmd_type == Command::kQuit)
                break;
        }
        catch (std::runtime_error& e) {
            PROGRAM_DEBUG("%s", e.what());
        }
    }

    // Join both reactor and proactor to quit.
    reactor_td.join();
    proactor_td.join();
    
	return 0;
}

