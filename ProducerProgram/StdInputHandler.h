#ifndef Std_INPUT_HANDLER_H
#define Std_INPUT_HANDLER_H
#include <istream>
#include <mutex>
#include <iostream>
#include <memory>
#include "Commands.h"

class StdInputHandler {
    static const std::string sCmdStartHeader;
    static const std::string sCmdStopHeader;
    static const std::string sCmdQuitHeader;
public:
    // Provides the singleton interface for this program.
    static StdInputHandler* instance();

    // Deallocates the singleton (if initialized).
    ~StdInputHandler();

    // GetInputFromStdStream()
    static std::string GetLine();

    std::shared_ptr<Command> MakeCommand(const std::string& input) const;

private:
    // Make StdInputHandler in program singleton.
    StdInputHandler();
    StdInputHandler(const StdInputHandler&);;
    StdInputHandler& operator=(const StdInputHandler&);;

    static Command::CommandType CheckCommandType(const std::string& cmd_type);
    std::shared_ptr<StartCommand> MakeStartCommand(const std::string& params_str) const;
    std::shared_ptr<StopCommand> MakeStopCommand(const std::string& params_str) const;
    std::shared_ptr<QuitCommand> MakeQuitCommand(const std::string& params_str) const;

    // Pointers to singleton handler
    static StdInputHandler* handler_;
    static std::once_flag once_flag_;

};

#endif
