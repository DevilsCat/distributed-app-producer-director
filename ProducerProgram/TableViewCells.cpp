#include "stdafx.h"
#include "TableViewCells.h"

//
// make keys for cell factory method.
#ifndef MAKE_KEYS
#define MAKE_KEYS_RETURN(...) \
    do { \
    const char* keys[] = { __VA_ARGS__ }; \
    return std::vector<std::string>(std::begin(keys), std::end(keys)); \
        } while(0)
#endif

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
    MAKE_KEYS_RETURN("Director_Id", "Name", "Status");
}

std::string PlayTableViewCell::get_value(const std::string& key) {
    if (key == "Director_Id") return std::to_string(director_id_);
    if (key == "Name")     return name_;
    if (key == "Status") {
        switch (status_) {
        case kAvailable:	return "available";
        case kUnavailable:	return "unavailable";
        case kInProgress:	return "in progress";
        default:			return std::string();
        }
    }
    return std::string();
}

double PlayTableViewCell::get_weight(const std::string& key) {
    if (key == "Director_Id") return 0.2;
    if (key == "Name")        return 0.6;
    if (key == "Status")      return 0.2;
    return 0.0;
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

double DebugTableViewCell::get_weight(const std::string& key) {
    if (key == "Message") return 1.0;
    return 0.0;
}

