#include "stdafx.h"
#include "ViewRenderer.h"
#include <windows.h>
#include <string>
#include <sstream>

ViewRenderer* ViewRenderer::renderer_ = nullptr;
std::once_flag ViewRenderer::once_flag_;

ViewRenderer* ViewRenderer::instance() {
    std::call_once(once_flag_, []{ renderer_ = new ViewRenderer; });
    return renderer_;
}

void ViewRenderer::ReceiveUserInput(const char& ch) {
    std::lock_guard<std::mutex> lk(user_mut_);
    if (ch == '\b') {  // backspace
        if (!user_buf_.empty()) user_buf_.pop_back();
        return;
    }
    user_buf_.push_back(ch);
}

void ViewRenderer::ClearUserInput() {
    std::lock_guard<std::mutex> lk(user_mut_);
    user_buf_.clear();
}

ViewRenderer::ViewRenderer() : std_out_(std::cout), cursor_pos_(0, 0) {}

ViewRenderer::ViewRenderer(const ViewRenderer&): ViewRenderer() {}

ViewRenderer& ViewRenderer::operator=(const ViewRenderer&) { return *this; }

int ViewRenderer::UpdateWindowSize() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int ret = GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    if (ret) {
        window_width_ = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        window_height_ = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }
    return ret;
}

void ViewRenderer::RenderPrompt() {
    GoToPromptPos();
    std::string prompt_line = truncate(std::string(PROMPT_MARK) + user_buf_, window_width_ - 1);
    std_out_ << left(prompt_line, window_width_, '=');
    GoToPromptPos(prompt_line.size());
}


void ViewRenderer::GoToXY(WidthType x, HeightType y) {
    COORD homeCoords = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), homeCoords);
}

void ViewRenderer::GoToPromptPos(WidthType x) {
    UpdateWindowSize(); // in order to get latest window height.
    GoToXY(x, window_height_ - PROMPT_OFFSET);
}

std::string ViewRenderer::left(const std::string s, const int w, const char delimiter) {
    std::stringstream ss, spaces;
    int padding = w - s.size() - 1;             // count excess room to pad
    for (int i = 0; i<padding; ++i)
        spaces << delimiter;
    ss << s << spaces.str();                    // format with padding
    return ss.str();
}

std::string ViewRenderer::center(const std::string& s, const int& w, const char delimiter) {
    std::stringstream ss, spaces;
    int padding = w - s.size() - 1;                 // count excess room to pad
    for (int i = 0; i<padding / 2; ++i)
        spaces << delimiter;
    ss << spaces.str() << s << spaces.str();    // format with padding
    if (padding>0 && padding % 2 != 0)               // if odd #, add 1 space
        ss << delimiter;
    return ss.str();
}

std::string ViewRenderer::truncate(const std::string& s, size_t w, bool show_ellipsis) {
    const std::string sEllipsis = "...";
    if (s.length() > w)
        return show_ellipsis ? s.substr(0, w - sEllipsis.size()) + sEllipsis : s.substr(0, w);
    return s;
}
