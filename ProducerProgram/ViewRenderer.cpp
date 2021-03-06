﻿// ViewRenderer.cpp -- This file defines a ViewRenderer class, used to render the view.
// Created by Yu Xiao, Anqi Zhang, Yuhan Hao, all rights reserved.
//
#include "stdafx.h"
#include "ViewRenderer.h"
#include "Utils.h"

using namespace utils;

std::string ViewRenderer::sNoMainView = "NO_MAIN_VIEW";
std::string ViewRenderer::sAllViews = "ALL_VIEWS";

std::shared_ptr<ViewRenderer> ViewRenderer::renderer_ = nullptr;
std::once_flag ViewRenderer::once_flag_;

ViewRenderer::ViewInfo::ViewInfo() :
view(nullptr), weight(0),
start_height(0), nline_max(0) {}

ViewRenderer::ViewInfo::ViewInfo(View* view_, double weight_) :
view(view_), weight(weight_),
start_height(0), nline_max(0) {}

// Initialization methods
ViewRenderer::ViewRenderer() : 
    prompt_view_(PromptView::MakeView()), hint_view_(HintView::MakeView("Hint")), curr_view_idx_(0) {}

ViewRenderer::ViewRenderer(const ViewRenderer&) {}

ViewRenderer& ViewRenderer::operator=(const ViewRenderer&) { return *this; }


ViewRenderer* ViewRenderer::instance() {
    std::call_once(once_flag_, []{ renderer_ = std::shared_ptr<ViewRenderer>(new ViewRenderer); });
    return renderer_.get();
}

void ViewRenderer::AddView(const std::string& name, View* view, double weight) {
    view_names_.push_back(name);
    view_info_map_[name] = ViewInfo(view, weight);
}

HintView* ViewRenderer::hint_view() const {
    return hint_view_.get();
}

PromptView* ViewRenderer::prompt_view() const {
    return prompt_view_.get();
}

void ViewRenderer::Render(const std::string& view_name) {
    std::lock_guard<std::mutex> lk(render_mut_);
    RenderAll_(view_name == sAllViews ||                    // If AllViews toggles, then render all,
               view_name == view_names_[curr_view_idx_]);   // otherwise, render the main view only if it's the current view.
}

void ViewRenderer::Render(View* view) {
    std::lock_guard<std::mutex> lk(render_mut_);
    RenderAll_(view == GetCurrentViewInfo_().view.get());   // Use the pointer value to check if identical.
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
    if (curr_view_idx_ == 0) { return; }  // no more previous view;
    --curr_view_idx_;
    RenderAll_(true);
}

void ViewRenderer::Scroll(bool is_up) {
    std::lock_guard<std::mutex> lk(render_mut_);
    UpdateWindowSize_();
    if (is_up)  GetCurrentViewInfo_().view->ScrollUp(window_height_ - WINDOW_HEIGHT_PRESERVED);
    else        GetCurrentViewInfo_().view->ScrollDown(window_height_ - WINDOW_HEIGHT_PRESERVED);
    RenderAll_(true);
}


View* ViewRenderer::GetView_(const std::string& name) {
    if (!view_info_map_.count(name)) { return nullptr; }  // view not exists.
    return view_info_map_[name].view.get();
}

void ViewRenderer::RenderAll_(bool render_main_view) {
    UpdateWindowSize_();
    if (render_main_view) {  // clear every thing and re-render.
        system("cls");       // clear the screen.
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
    short cursor_x, cursor_y;  // Stores old cursor position, enable the cursor back.
    windows::GetCursorPos(cursor_x, cursor_y);
    windows::GoToXY(0, window_height_ - HINT_OFFSET);  // Go to hint view position
    hint_view_->Draw(window_width_, window_height_ - WINDOW_HEIGHT_PRESERVED);
    windows::GoToXY(cursor_x, cursor_y);
}

void ViewRenderer::RenderPromptView_() const {
    windows::GoToXY(0, window_height_ - PROMPT_OFFSET);  // Go to Prompt view position. 
    prompt_view_->Draw(window_width_, window_height_ - WINDOW_HEIGHT_PRESERVED);
}

// Util methods.
int ViewRenderer::UpdateWindowSize_() {
    return windows::GetWindowSize(window_width_, window_height_);
}

ViewRenderer::ViewInfo& ViewRenderer::GetCurrentViewInfo_() {
    return view_info_map_[view_names_[curr_view_idx_]];
}
