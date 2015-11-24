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

    // Make StdInputHandler in program singleton.
    StdInputHandler(const StdInputHandler&) = delete;
    StdInputHandler& operator= (const StdInputHandler&) = delete;

    // GetInputFromStdStream()
    static std::string GetLine();

    std::shared_ptr<Command> MakeCommand(const std::string& input) const;

private:
    StdInputHandler() : std_cin_(std::cin) {}

    static Command::CommandType CheckCommandType(const std::string& cmd_type);
    std::shared_ptr<StartCommand> MakeStartCommand(const std::string& params_str) const;
    std::shared_ptr<StopCommand> MakeStopCommand(const std::string& params_str) const;
    std::shared_ptr<QuitCommand> MakeQuitCommand(const std::string& params_str) const;

    // input stream to read from.
    std::istream& std_cin_;

    // Pointers to singleton handler
    static StdInputHandler* handler_;
    static std::once_flag once_flag_;

};

#endif
