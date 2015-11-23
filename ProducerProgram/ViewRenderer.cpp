#include "stdafx.h"
#include "ViewRenderer.h"
#include "Utils.h"
#include <windows.h>

using namespace utils;

std::string ViewRenderer::sNoMainView = "NO_MAIN_VIEW";
std::string ViewRenderer::sAllViews = "ALL_VIEWS";

ViewRenderer* ViewRenderer::renderer_ = nullptr;
std::once_flag ViewRenderer::once_flag_;

// Initialization methods
ViewRenderer::ViewRenderer() : 
    prompt_view_(PromptView::MakeView()), hint_view_(HintView::MakeView("Hint")) 
{}

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

HintView* ViewRenderer::hint_view() const {
    return hint_view_.get();
}

PromptView* ViewRenderer::prompt_view() const {
    return prompt_view_.get();
}

void ViewRenderer::Render(const std::string& view_name) {
    std::lock_guard<std::mutex> lk(render_mut_);
    // render only when this view is presented.
    RenderAll_(view_name == sAllViews || 
               view_name == view_names_[curr_view_idx_]);
}

void ViewRenderer::Render(View* view) {
    std::lock_guard<std::mutex> lk(render_mut_);
    RenderAll_(view == GetCurrentViewInfo_().view.get());
}

void ViewRenderer::RenderHintView() {
    std::lock_guard<std::mutex> lk(render_mut_);
    RenderHintView_();
}

void ViewRenderer::RenderPromptView() {
    std::lock_guard<std::mutex> lk(render_mut_);
    RenderPromptView_();
}

void ViewRenderer::NextView() {
    std::lock_guard<std::mutex> lk(render_mut_);
    if (curr_view_idx_ >= view_names_.size() - 1) { return; } // no more next view.
    ++curr_view_idx_;
    RenderAll_(true);
}

void ViewRenderer::PrevView() {
    std::lock_guard<std::mutex> lk(render_mut_);
    if (curr_view_idx_ == 0) { return; }  // nor more previous view;
    --curr_view_idx_;
    RenderAll_(true);
}

void ViewRenderer::RenderAll_(bool to_render_view) {
    UpdateWindowSize_();
    if (to_render_view) {  // clear every thing and re-render.
        system("cls");  // clear the screen.
        RenderCurrView_();
    }
    RenderHintView_();
    RenderPromptView_();
}

void ViewRenderer::RenderCurrView_() {
    if (view_names_.size() == 0)  { return; }  // no view added yet.
    if (curr_view_idx_ >= view_names_.size()) { return; }  // but this should not happen.
    RenderView_(view_info_map_[view_names_[curr_view_idx_]], false);
}

void ViewRenderer::RenderView_(const ViewInfo& vi, bool cursor_back) const {
    short cursor_x, cursor_y;
    windows::GetCursorPos(cursor_x, cursor_y);
    windows::GoToXY(0, vi.start_height);
    vi.view->Draw(window_width_, window_height_ - WINDOW_HEIGHT_PRESERVED);
    if (cursor_back) {
        windows::GoToXY(cursor_x, cursor_y);
    }
}

void ViewRenderer::RenderHintView_() const {
    short cursor_x, cursor_y;  // enable the cursor back.
    windows::GetCursorPos(cursor_x, cursor_y);
    GoToHintPos();
    hint_view_->Draw(window_width_, window_height_ - WINDOW_HEIGHT_PRESERVED);
    windows::GoToXY(cursor_x, cursor_y);
}

void ViewRenderer::RenderPromptView_() const {
    GoToPromptPos();
    prompt_view_->Draw(window_width_, window_height_ - WINDOW_HEIGHT_PRESERVED);
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

void ViewRenderer::GoToHintPos() const {
    windows::GoToXY(0, window_height_ - HINT_OFFSET);
}

void ViewRenderer::GoToPromptPos(WidthType x) const {
    //UpdateWindowSize_(); // in order to get latest window height.
    windows::GoToXY(x, window_height_ - PROMPT_OFFSET);
}

ViewRenderer::ViewInfo& ViewRenderer::GetCurrentViewInfo_() {
    return view_info_map_[view_names_[curr_view_idx_]];
}
