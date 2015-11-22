#pragma once
#include "View.h"
#include "Utils.h"

#define PROMPT_MARK      ">>"
class PromptView : public View {
public:
    void Draw(const short& width) const override {
        DrawPrompt_(width);
    }

    void ReceiveUserInput(const char& ch) {
        if (ch == '\b') {  // backspace
            if (!user_buf_.empty()) user_buf_.pop_back();
            return;
        }
        user_buf_.push_back(ch);
    }

    void ClearUserInput() {
        user_buf_.clear();
    }

    static PromptView* MakeView() {
        return new PromptView(std::string());
    }

private:
    PromptView(const std::string& title) : View(title) {}

    void DrawPrompt_(const short& width) const{
        short old_x, old_y;
        windows::GetCursorPos(old_x, old_y);  // save for come back after printing the prompt.
        std::string prompt_line = windows::truncate(std::string(PROMPT_MARK) + user_buf_, width - 1);
        std::cout << windows::left(prompt_line, width, ' ');
        windows::GoToXY(prompt_line.size(), old_y);
    }

    std::string user_buf_;
};
