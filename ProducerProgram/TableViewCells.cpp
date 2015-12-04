// TableViewCell.cpp -- This files defines Table View Cell class family.
// Created by Yu Xiao, Anqi Zhang, Yuhan Hao
//
#include "stdafx.h"
#include "TableViewCells.h"

// factory method Macro for making keys for cell 
#ifndef MAKE_KEYS
#define MAKE_KEYS_RETURN(...) \
    do { \
    const char* keys[] = { __VA_ARGS__ }; \
    return std::vector<std::string>(std::begin(keys), std::end(keys)); \
        } while(0)
#endif

// Macro for Play cells.
#define PLAY_DIRECTOR_ID     "Director_Id"
#define PLAY_NAME            "Name"
#define PLAY_STATUS          "Status"
#define WEIGHT_DIRECTOR_ID   0.2
#define WEIGTH_NAME          0.6
#define WEIGHT_STATUS        0.2
const char* kStatusStrArr[] = { "available", "unavailable", "in progress" };

// Macro for Debug cells.
#define DEBUG_MESSAGE        "Message"
#define WEIGTH_MESSAGE       1.0
#define WEIGHT_DEFAULT       0.0

PlayTableViewCell::PlayTableViewCell() : director_id_(), play_id_(), name_(), status_(kAvailable) {}

PlayTableViewCell::PlayTableViewCell(const int& director_id, const int& play_id, const std::string& name, const StatusType status) :
director_id_(director_id), play_id_(play_id), name_(name), status_(status)
{}

void PlayTableViewCell::set_director_id(const int& director_id) {
    director_id_ = director_id;
}

void PlayTableViewCell::set_play_id(const int& play_id) {
    play_id_ = play_id;
}

void PlayTableViewCell::set_name(const std::string& name) {
    name_ = name;
}

void PlayTableViewCell::set_status(const StatusType status) {
    status_ = status;
}

int PlayTableViewCell::director_id() const {
    return director_id_;
}

int PlayTableViewCell::play_id() const {
    return play_id_;
}

std::string PlayTableViewCell::name() const {
    return name_;
}

PlayTableViewCell::StatusType PlayTableViewCell::status() const {
    return status_;
}

std::vector<std::string> PlayTableViewCell::get_keys() {
    MAKE_KEYS_RETURN(PLAY_DIRECTOR_ID, PLAY_NAME, PLAY_STATUS);
}

std::string PlayTableViewCell::get_value(const std::string& key) {
    if (key == PLAY_DIRECTOR_ID)  return std::to_string(director_id_);
    if (key == PLAY_NAME)         return name_;
    if (key == PLAY_STATUS)       return kStatusStrArr[status_];
    return std::string();
}

double PlayTableViewCell::get_weight(const std::string& key) {
    if (key == PLAY_DIRECTOR_ID) return WEIGHT_DIRECTOR_ID;
    if (key == PLAY_NAME)        return WEIGTH_NAME;
    if (key == PLAY_STATUS)      return WEIGHT_STATUS;
    return WEIGHT_DEFAULT;
}

DebugTableViewCell::DebugTableViewCell() {}

DebugTableViewCell::DebugTableViewCell(const std::string& message) : message_(message) {}

std::vector<std::string> DebugTableViewCell::get_keys() {
    MAKE_KEYS_RETURN(DEBUG_MESSAGE);
}

std::string DebugTableViewCell::get_value(const std::string& key) {
    if (key == DEBUG_MESSAGE) return message_;
    return std::string();
}

double DebugTableViewCell::get_weight(const std::string& key) {
    if (key == DEBUG_MESSAGE) return WEIGTH_MESSAGE;
    return WEIGHT_DEFAULT;
}

