#include "stdafx.h"
#include "StdInputHandler.h"
#include "Utils.h"
#include <string>
#include <vector>

StdInputHandler* StdInputHandler::handler_ = nullptr;

std::once_flag StdInputHandler::once_flag_;

StdInputHandler* StdInputHandler::instance() {
    call_once(once_flag_, []{
                  if (handler_ == nullptr)
                      handler_ = new StdInputHandler();
              });
    return handler_;
}

StdInputHandler::~StdInputHandler() {
    if (handler_) {
        delete handler_;
    }
}

std::string StdInputHandler::GetLine() const {
    std::string line;
    std::getline(std_cin_, line);
    return line;
}

std::shared_ptr<Command> StdInputHandler::MakeCommand(const std::string& input) const {
    if (input.empty()) { return nullptr; }
    std::istringstream iss(input);
    std::string cmd_type, params;
    iss >> cmd_type >> std::ws;
    std::getline(iss, params);

    switch (CheckCommandType(cmd_type)) {
    case Command::kStart: 
        return MakeStartCommand(params);
    case Command::kStop: 
        return MakeStopCommand(params);
    case Command::kQuit: 
        return MakeQuitCommand(params);
    case Command::kUnknown: 
    default:
        return nullptr;
    }
}

Command::CommandType StdInputHandler::CheckCommandType(const std::string& cmd_type) {
    if (cmd_type == "start")   return Command::kStart;
    if (cmd_type == "stop")    return Command::kStop;
    if (cmd_type == "quit")    return Command::kQuit;
    return Command::kUnknown;
}

std::shared_ptr<StartCommand> StdInputHandler::MakeStartCommand(const std::string& params_str) const {
    const unsigned kNumParams = 1;
    const unsigned kNumberPos = 0;
    std::vector<std::string> params = utils::tokenize(params_str);
    if (params.size() != kNumParams)
        throw std::runtime_error("Parse Start Command: Numbers of Parameters Mismatch.");
    if (!utils::is_number(params[kNumberPos]))
        throw std::runtime_error("Parse Start Command: Invalid Number Input.");
    return std::make_shared<StartCommand>(utils::to_number(params[kNumberPos]));
}

std::shared_ptr<StopCommand> StdInputHandler::MakeStopCommand(const std::string& params_str) const {
    const unsigned kNumParams = 1;
    const unsigned kNumberPos = 0;
    std::vector<std::string> params = utils::tokenize(params_str);
    if (params.size() != kNumParams)
        throw std::runtime_error("Parse Stop Command: Numbers of Parameters Mismatch.");
    if (!utils::is_number(params[kNumberPos]))
        throw std::runtime_error("Parse Start Command: Invalid Number Input.");
    return std::make_shared<StopCommand>(utils::to_number(params[kNumberPos]));
}

std::shared_ptr<QuitCommand> StdInputHandler::MakeQuitCommand(const std::string& params_str) const {
    if (!params_str.empty())
        throw std::runtime_error("Parse Quit Command: Numbers of Paramters Mismatch.");
    return std::make_shared<QuitCommand>();
}
