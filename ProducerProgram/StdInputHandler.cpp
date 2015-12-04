//
// Decclares a class to analyse user input commands and make corresponding
// Command objects.
//
#include "stdafx.h"
#include "StdInputHandler.h"
#include "Utils.h"
#include <string>
#include <vector>
#include <conio.h>
#include "Views.h"
#include "ViewRenderer.h"

// Macro for key code.
#define KEY_SIGINT        03
#define KEY_UP            72
#define KEY_DOWN          80
#define KEY_LEFT          75
#define KEY_RIGHT         77
#define KEY_ARROW_PREFIX  224

const std::string StdInputHandler::sCmdStartHeader = "start";
const std::string StdInputHandler::sCmdStopHeader = "stop";
const std::string StdInputHandler::sCmdQuitHeader = "quit";

StdInputHandler* StdInputHandler::handler_ = nullptr;
std::once_flag StdInputHandler::once_flag_;

StdInputHandler* StdInputHandler::instance() {
    call_once(once_flag_, []{ handler_ = new StdInputHandler; });
    return handler_;
}

StdInputHandler::~StdInputHandler() {
    if (handler_) {
        delete handler_;
    }
}

std::string StdInputHandler::GetLine() {
    std::string line;
    int ch;
    PromptView* prompt_view = ViewRenderer::instance()->prompt_view();
    do {
        ch = _getch();
        if (ch == KEY_SIGINT)            { return sCmdQuitHeader; }  // recover the ctrl-c function.
        else if (ch == KEY_ARROW_PREFIX) {}  // ignore arrow prefix. 
        else if (ch == KEY_UP)           { ViewRenderer::instance()->Scroll(true); }
        else if (ch == KEY_DOWN)         { ViewRenderer::instance()->Scroll(false); }
        else if (ch == KEY_LEFT)         { ViewRenderer::instance()->PrevView(); } 
        else if (ch == KEY_RIGHT)        { ViewRenderer::instance()->NextView(); }
        else if (ch != '\r')             { ViewRenderer::instance()->prompt_view()->AddChar(ch); }
        else {  // user hits a return.
            line = prompt_view->user_buf();  // get the input line from user.
            prompt_view->ClearUserInput();
            ViewRenderer::instance()->hint_view()->set_hint(HintView::kHintDefault);
            ViewRenderer::instance()->Render(ViewRenderer::sNoMainView);
            break;
        }
        ViewRenderer::instance()->Render(ViewRenderer::sNoMainView);
    } while (true);
    return line;
}

std::shared_ptr<Command> StdInputHandler::MakeCommand(const std::string& input) const {
    if (input.empty()) { return nullptr; }
    std::istringstream iss(input);
    std::string cmd_type, params;
    iss >> cmd_type >> std::ws;
    std::getline(iss, params);

    switch (CheckCommandType(cmd_type)) {
    case Command::kStart:   { return MakeStartCommand(params); }
    case Command::kStop:    { return MakeStopCommand(params); }
    case Command::kQuit:    { return MakeQuitCommand(params); }
    case Command::kUnknown: { return nullptr; }
    default:                { return nullptr; }
    }
}

StdInputHandler::StdInputHandler() {}

StdInputHandler::StdInputHandler(const StdInputHandler&) {}

StdInputHandler& StdInputHandler::operator=(const StdInputHandler&) { return *this; }



Command::CommandType StdInputHandler::CheckCommandType(const std::string& cmd_type) {
    if (cmd_type == sCmdStartHeader)   return Command::kStart;
    if (cmd_type == sCmdStopHeader)    return Command::kStop;
    if (cmd_type == sCmdQuitHeader)    return Command::kQuit;
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
