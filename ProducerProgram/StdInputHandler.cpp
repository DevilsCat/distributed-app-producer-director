#include "stdafx.h"
#include "StdInputHandler.h"
#include "Utils.h"
#include <string>
#include <vector>
#include <conio.h>
#include "Views.h"
#include "ViewRenderer.h"
#include <csignal>

#define KEY_UP      72
#define KEY_DOWN    80
#define KEY_LEFT    75
#define KEY_RIGHT   77
#define KEY_ARROW_PREFIX  224

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
    int ch;
    PromptView* prompt_view = ViewRenderer::instance()->prompt_view();
    HintView* hint_view = dynamic_cast<HintView*>(ViewRenderer::instance()->hint_view());
    do {
        ch = _getch();
        if (ch == 0x03) raise(SIGINT);  // recover the ctrl-c function.
        else if (ch == KEY_ARROW_PREFIX) {}  // ignore arrow prefix. 
        else if (ch == KEY_UP) { hint_view->set_hint("TODO: paging up"); }   // so far no-ops
        else if (ch == KEY_DOWN) { hint_view->set_hint("TODO: paging down"); } // so far no-ops
        else if (ch == KEY_LEFT) { hint_view->set_hint("TODO: switching tag prev"); } // ViewRenderer::instance()->PrevView();
        else if (ch == KEY_RIGHT) { hint_view->set_hint("TODO: switching tag next"); } // ViewRenderer::instance()->NextView();
        else if (ch == '\r') {
            line = prompt_view->user_buf();  // get the input line from user.
            prompt_view->ClearUserInput();
            hint_view->set_hint(HintView::kHintDefault);
            ViewRenderer::instance()->Render();
            break;
        }
        else { prompt_view->ReceiveUserInput(ch); }
        ViewRenderer::instance()->RenderPromptView();
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
