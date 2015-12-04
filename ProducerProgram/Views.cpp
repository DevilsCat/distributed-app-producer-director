// Views.cpp -- This file defines View class family which customize draw routine on screen.
// Created by Anqi Zhang, Yu Xiao, Yuhan Hao, all rights reserved.
//
#include "stdafx.h"
#include "Views.h"

using namespace utils;

const char* HintView::kHintDefault = "";
const char* PromptView::sPromptMark = ">>";

View::View(const std::string& title) : title_(title) {}

View::~View() {}

void View::ScrollUp(const short& nline_scroll) {
    // no-ops
}

void View::ScrollDown(const short& nline_scroll) {
    // no-ops
}

void View::DrawTitle_(const short& width) const {
    std::cout << utils::windows::left(title_, width, '=') << std::endl;
}

void HintView::Draw(const short& width, const short& height) const {
    std::lock_guard<std::mutex> lk(m_);
    DrawTitle_(width);
    DrawHint_(width);
}

HintView* HintView::MakeView(const std::string& title) {  // The reason I use Factory method in Views,
    return new HintView(title);                           // is to ensure we always create the View on heap.
}

std::string HintView::hint() const {
    std::lock_guard<std::mutex> lk(m_);
    return hint_;
}

void HintView::set_hint(const std::string& hint) {
    std::lock_guard<std::mutex> lk(m_);
    hint_ = hint;
}

HintView::HintView(const std::string& title):
    View(title) 
{}

void HintView::DrawHint_(const short& width) const {
    std::cout << windows::center(hint_, width, ' ');
}

PromptView* PromptView::MakeView() {
    return new PromptView(std::string());
}

void PromptView::Draw(const short& width, const short& height) const {
    DrawPrompt_(width);
}

void PromptView::AddChar(const char& ch) {
    if (ch == '\b') {  // backspace
        if (!user_buf_.empty()) user_buf_.pop_back();  // performs delete the tail char.
        return;
    }
    user_buf_.push_back(ch);
}

std::string PromptView::user_buf() const {
    return user_buf_;
}

void PromptView::ClearUserInput() {
    user_buf_.clear();
}

PromptView::PromptView(const std::string& title): View(title) {}

void PromptView::DrawPrompt_(const short& width) const {
    short old_x, old_y;
    windows::GetCursorPos(old_x, old_y);  // save for come back after printing the prompt.
    std::string prompt_line = windows::truncate(std::string(sPromptMark) + user_buf_, width - 1);
    std::cout << windows::left(prompt_line, width, ' ');
    windows::GoToXY(prompt_line.size(), old_y);
}

