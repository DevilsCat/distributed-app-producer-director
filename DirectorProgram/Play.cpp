// Play.cpp -- This file implements a Play class, which allows players to enter and "play".
// for all "Players" to have dialogs in order.
// Created by Anqi Zhang, Yu Xiao, Yuhan Hao
//
#include "stdafx.h"
#include "Play.h"
#include <iostream>
#include <mutex>
#include "InterruptFlag.h"
#include "ProgramException.h"

Play::Play(const std::vector<std::string>& scene_titles,
           const std::vector<unsigned>& required_pnum_v) :
    line_counter_(1), frag_counter_(1), on_stage_(0), required_pnum_v_(required_pnum_v),
    progress_(PLAY_BEGIN)
{
	// Initialize the iterator (scene_title_iterator_) point to the beginning of that container
    scene_titles_ = scene_titles;
	scene_title_iterator_ = scene_titles_.begin();
}

// Note: Not very different from the previous lab, but we need a conditional 
// variable to check if it is the right scene and fragment.
void Play::Recite(std::map<play_t::number_t, play_t::dialog_t>::iterator &it, const unsigned & frag_counter) {
    // Retrieve data from contents data structure.
    play_t::number_t number = it->first;
    play_t::name_t   name = it->second.first;
    play_t::content_t content = it->second.second;
    
    std::unique_lock<std::mutex> lk(recite_m_);
    InterruptibleWait(recite_cond_, lk, [&]{ return frag_counter_ == frag_counter && number <= line_counter_; });
    
    // If badly formed script fragment.
    if (number < line_counter_) {
        std::cerr << "Line " << number << std::ends << name << std::ends << content << " is skipped." << std::endl;
        recite_cond_.notify_all();
        return;
    }
    
    AnnonceRoleName(name);
    // Player recites its content.
    std::cout << content << std::endl;

    ++line_counter_;
    last_one_acting_ = name;
    
    recite_cond_.notify_all();
}

void Play::Enter(const unsigned& scene_frag_i) {
	std::unique_lock<std::mutex> lk(enter_m_);
    InterruptibleWait(stage_cond_, lk, [&]{ return scene_frag_i <= frag_counter_; });
    
    if (scene_frag_i < frag_counter_) {  // This should not happens since the program should make sure
        throw ProgramException(          // when Player can enter here, if and only if Play's fragment
            "Scene Fragment Invalid",    // counter is the smallest.
            ProgramException::SCENE_FRAGMENT_INVALID
        );
    }
}

void Play::EnterStage(const std::string& name) {
    std::unique_lock<std::mutex> lk(role_switch_m_);
    // Instead of increase on_stage counter inside Enter, we added 
    // one section of barrier synchronization after entering stage.
    ++on_stage_;

    // Once this Player enter the stage, he should be recorded into
    // "Who is now on the stage" data structure.
    now_who_on_stage[name] = true;

    // We retrieve the required players number for this fragment used
    // conditional wait criteria.
    const size_t kOffsetAdjust = 1;
    unsigned int required_pnum = required_pnum_v_[frag_counter_ - kOffsetAdjust]; // since frag_counter start from 1.

    if (on_stage_ != required_pnum) {
        InterruptibleWait(role_switch_cond_, lk, [&]()->bool{ return on_stage_ == required_pnum; });
    } else {
        AnnouncePlayProgress();
        role_switch_cond_.notify_all();
    }

}

void Play::Exit() {
    std::lock_guard<std::mutex> lk(exit_m_);
    const unsigned kNumLastOneOnStage = 1;
	if (on_stage_ > kNumLastOneOnStage) 
        --on_stage_;
	else if (on_stage_ < kNumLastOneOnStage)  // Again this should not happened
        throw ProgramException(               // If this happned, it means some how
            "On_stage Counter Invalid.",      // a Player Exit without Enter (a fatal program error).
            ProgramException::ON_STAGE_COUNTER_INVALID
        );
    else {  // This fragment is finished
        MoveToNextFrag();

        if (scene_title_iterator_ == scene_titles_.end()) {  // Indicate Play is end.
            progress_ = PLAY_END;
            AnnouncePlayProgress();  // performs last time announce here, since no thread will call EnterStage again.
        } else if (!scene_title_iterator_->empty()) {  // New scene
            progress_ = SCENE_SWITCH;
        } else { // New fragment.
            progress_ = FRAG_SWITCH;
        }

        // Since all Players exits, records them into "who are previously on stage", so that
        // when new "who are now on stage" produces, Play can tell who indeed exits, or just "faking" exits.
        prev_who_on_stage = now_who_on_stage;
        now_who_on_stage.clear();
        
        stage_cond_.notify_all();
	}

}

void Play::Reset() {
    line_counter_ = 1; 
    frag_counter_ = 1;
    on_stage_     = 0;
    scene_title_iterator_ = scene_titles_.begin();
}

void Play::MoveToNextFrag() {
    --on_stage_;
    ++frag_counter_;
    line_counter_ = 1; // Reset the line counter.
    last_one_acting_.clear(); // Empty the last speaking.
    ++scene_title_iterator_;
}

void Play::AnnouncePlayProgress() {
    switch (progress_) {
    case PLAY_BEGIN: 
        AnnounceScene(*scene_title_iterator_);
        AnnounceEnter(now_who_on_stage);
        break;
    case FRAG_SWITCH: 
        AnnounceRoleSwitch();
        break;
    case SCENE_SWITCH: 
        AnnounceExit(prev_who_on_stage);
        AnnounceScene(*scene_title_iterator_);
        AnnounceEnter(now_who_on_stage);
        break;
    case PLAY_END: 
        AnnounceExit(now_who_on_stage);
        break;
    default: break;
    }
}

void Play::AnnounceRoleSwitch() {
    std::cout << std::endl;
    for (auto prev_pr : prev_who_on_stage) {
        if (!now_who_on_stage.count(prev_pr.first))
            AnnounceExit(prev_pr.first);
    }
    for (auto now_pr : now_who_on_stage) {
        if (!prev_who_on_stage.count(now_pr.first))
            AnnounceEnter(now_pr.first);
    }
    std::cout << std::endl; // make annoucement seperate from scene or other fragments.
}

void Play::AnnounceEnter(const std::map<std::string, bool>& who) {
    std::cout << std::endl;
    for (auto str_bool_pair : who) {
        AnnounceEnter(str_bool_pair.first);
    }
    std::cout << std::endl;
}

void Play::AnnounceExit(const std::map<std::string, bool>& who) {
    std::cout << std::endl;
    for (auto str_bool_pair : who) {
        AnnounceExit(str_bool_pair.first);
    }
    std::cout << std::endl;
}

void Play::AnnounceScene(const std::string& scene_str) {
    if (scene_str.empty()) { return; }
    std::cout << scene_str << std::endl;
}

void Play::AnnonceRoleName(const std::string& name) {
    if (last_one_acting_.empty()) { // first player speaking
        std::cout << name << "." << std::endl;
    }
    else if (name.compare(last_one_acting_) != 0) {
        std::cout << "\n" << name << "." << std::endl;
    }
}

void Play::AnnounceEnter(const std::string& name) {
    std::cout << "[Enter " << name << ".]" << std::endl;
}

void Play::AnnounceExit(const std::string& name) {
    std::cout << "[Exit " << name << ".]" << std::endl;
}