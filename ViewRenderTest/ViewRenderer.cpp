#include "stdafx.h"
#include "ViewRenderer.h"
#include "Utils.h"
#include <windows.h>
#include <string>

using namespace utils;

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

void ViewRenderer::AddView(const std::string& name, View* view) {
    view_names_.push_back(name);
    view_map_[name] = std::unique_ptr<View>(view);
}

View* ViewRenderer::GetView(const std::string& name) {
    if (view_map_.count(name) == 0) { return nullptr; }
    return view_map_[name].get();
}

void ViewRenderer::Render() {
    std::lock_guard<std::recursive_mutex> lk(render_mut_);
    system("cls");  // clear the screen.
    UpdateWindowSize();
    RenderViews();
    RenderPrompt();
}

void ViewRenderer::RenderViews() {
    for (std::string& view_name : view_names_) {
        view_map_[view_name]->Draw(window_width_);
    }
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
    std::lock_guard<std::recursive_mutex> lk(render_mut_);  // since input/output thread will both use this function.
    GoToPromptPos();
    std::string prompt_line = windows::truncate(std::string(PROMPT_MARK) + user_buf_, window_width_ - 1);
    std_out_ << windows::left(prompt_line, window_width_, '=');
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


