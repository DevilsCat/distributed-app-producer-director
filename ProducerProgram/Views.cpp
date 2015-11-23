﻿#include "stdafx.h"
#include "Views.h"

using namespace utils;

//
// make keys for cell factory method.
#ifndef MAKE_KEYS
#define MAKE_KEYS_RETURN(...) \
    do { \
    const char* keys[] = { __VA_ARGS__ }; \
    return std::vector<std::string>(std::begin(keys), std::end(keys)); \
    } while(0)
#endif

const char* HintView::kHintDefault = "";
const char* PromptView::sPromptMark = ">>";

void HintView::Draw(const short& width) const {
    std::lock_guard<std::mutex> lk(m_);
    DrawTitle_(width);
    DrawHint_(width);
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

void PromptView::Draw(const short& width) const {
    DrawPrompt_(width);
}

void PromptView::ReceiveUserInput(const char& ch) {
    if (ch == '\b') {  // backspace
        if (!user_buf_.empty()) user_buf_.pop_back();
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

PlayTableViewCell::PlayTableViewCell(): director_(), name_(), status_(false) {}

PlayTableViewCell::PlayTableViewCell(const std::string& director, const std::string& name, bool status):
    director_(director), name_(name), status_(status) 
{}

void PlayTableViewCell::set_director(const std::string& director) {
    director_ = director;
}

void PlayTableViewCell::set_name(const std::string& name) {
    name_ = name;
}

void PlayTableViewCell::set_status(const bool status) {
    status_ = status;
}

std::vector<std::string> PlayTableViewCell::get_keys() {
    MAKE_KEYS_RETURN("Director", "Name", "Status");
}

std::string PlayTableViewCell::get_value(const std::string& key) {
    if (key == "Director") return director_;
    if (key == "Name")     return name_;
    if (key == "Status")   return status_ ? "available" : "inavailable";
    return std::string();
}

DebugTableViewCell::DebugTableViewCell() {}

DebugTableViewCell::DebugTableViewCell(const std::string& message) : message_(message) {}

std::vector<std::string> DebugTableViewCell::get_keys() {
    MAKE_KEYS_RETURN("Message");
}

std::string DebugTableViewCell::get_value(const std::string& key) {
    if (key == "Message") return message_;
    return std::string();
}