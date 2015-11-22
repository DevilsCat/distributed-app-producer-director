#include "stdafx.h"
#include "ViewRenderer.h"
#include "Utils.h"
#include <windows.h>
#include <string>

using namespace utils;

ViewRenderer* ViewRenderer::renderer_ = nullptr;
std::once_flag ViewRenderer::once_flag_;

// Initialization methods
ViewRenderer::ViewRenderer() : std_out_(std::cout), prompt_(PromptView::MakeView()) {}

ViewRenderer::ViewRenderer(const ViewRenderer&) : ViewRenderer() {}

ViewRenderer& ViewRenderer::operator=(const ViewRenderer&) { return *this; }

ViewRenderer* ViewRenderer::instance() {
    std::call_once(once_flag_, []{ renderer_ = new ViewRenderer; });
    return renderer_;
}

void ViewRenderer::AddView(const std::string& name, View* view, double weight) {
    view_names_.push_back(name);
    view_info_map_[name] = ViewInfo(view, weight);
}

View* ViewRenderer::GetView(const std::string& name) {
    if (view_info_map_.count(name) == 0) { return nullptr; }
    return view_info_map_[name].view.get();
}

PromptView* ViewRenderer::GetPrompt() const {
    return prompt_.get();
}

void ViewRenderer::Render() {
    std::lock_guard<std::mutex> lk(render_mut_);
    RenderAll_();
}

void ViewRenderer::Render(const std::string& view_name) {
    std::lock_guard<std::mutex> lk(render_mut_);
    RenderView_(view_info_map_[view_name], true);
}

void ViewRenderer::Render(View* view) {
    std::lock_guard<std::mutex> lk(render_mut_);
    for (auto name_vi_pair : view_info_map_) {
        if (view == name_vi_pair.second.view.get())  // search linearly for the view.
            RenderView_(name_vi_pair.second, true);
    }
}

void ViewRenderer::RenderPrompt() {
    std::lock_guard<std::mutex> lk(render_mut_);
    RenderPrompt_();
}

//void ViewRenderer::NextView() {
//    std::lock_guard<std::mutex> lk(render_mut_);
//    if (curr_view_idx_ >= view_names_.size() - 1) { return; } // no more next view.
//    ++curr_view_idx_;
//    RenderAll_();
//}

//void ViewRenderer::PrevView() {
//    std::lock_guard<std::mutex> lk(render_mut_);
//    if (curr_view_idx_ == 0) { return; }  // nor more previous view;
//    --curr_view_idx_;
//    RenderAll_();
//}

void ViewRenderer::RenderAll_() {
    system("cls");  // clear the screen.
    OnWindowChanged_();
    RenderViews_();
    RenderPrompt_();
}

void ViewRenderer::RenderViews_() {
    for (std::string view_name : view_names_) {
        RenderView_(view_info_map_[view_name], false);
    }
    //if (view_names_.size() == 0)  { return; }  // no view added yet.
    //if (curr_view_idx_ >= view_names_.size()) { return; }  // but this should not happen.
    //view_info_map_[view_names_[curr_view_idx_]]->Draw(window_width_);
}

void ViewRenderer::RenderView_(const ViewInfo& vi, bool cursor_back) const {
    short cursor_x, cursor_y;
    windows::GetCursorPos(cursor_x, cursor_y);
    windows::GoToXY(0, vi.start_height);
    vi.view->Draw(window_width_);
    if (cursor_back) {
        windows::GoToXY(cursor_x, cursor_y);
    }
}

void ViewRenderer::RenderPrompt_() {
    GoToPromptPos();
    prompt_->Draw(window_width_);
}

void ViewRenderer::OnWindowChanged_() {
    UpdateWindowSize_();
    UpdateViewStartPos_();
}

void ViewRenderer::UpdateViewStartPos_() {
    short pos = 0;
    for (std::string view_name : view_names_) {
        ViewInfo& vi = view_info_map_[view_name];
        vi.start_height = pos;
        pos += short(window_height_ * vi.weight);
    }
}

// Util methods.
int ViewRenderer::UpdateWindowSize_() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int ret = GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    if (ret) {
        window_width_ = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        window_height_ = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }
    return ret;
}

void ViewRenderer::GoToPromptPos(WidthType x) {
    UpdateWindowSize_(); // in order to get latest window height.
    windows::GoToXY(x, window_height_ - PROMPT_OFFSET);
}


