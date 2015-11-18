// Director.cpp -- Implements a Director class which read the Script tree and assigned role to Player and also
// make sure interrupt or wait for play progress
// Created by Yu Xiao, Yuhan Hao, Anqi Zhang
//
#include "stdafx.h"
#include "Director.h"
#include "utils.h"

#define MAX(X,Y)    ((X) > (Y) ? (X) : (Y))

Director::Director(std::string const& script_file_string) :
    Director(script_file_string, 0)
{}

Director::~Director() {
    try {
        // wait all parts to be finished, interrupts
        // threads if any exception is thrown.
        WaitForAllPartsDone();
    }
    catch (ProgramException&) {
        // Invocate an interruption to those waiting threads.
        for_each(players_.begin(), players_.end(), std::mem_fn(&Player::InterruptCurrentPlay));
    }

    // Invocate a terminating signal to those "idle" waiting threads.
    for_each(players_.begin(), players_.end(), std::mem_fn(&Player::Exit));
    
}

Director::Director(std::string const& script_file_string, unsigned int minimum_players) :
    visit_ops_t(kSKIM),
	script_file_string_(script_file_string), 
    minimum_players_(minimum_players), 
    maximum_players_(0),
    parser_(script_file_string_),
    frag_counter(0),
    player_idx_(0)
{
    // Use script parser to parse the input file, and stores in script tree.
    parser_.Parse();

    script_ptr_ = parser_.script_ptr();

    // Visit the script AST tree to get scene titles
    script_ptr_->accept(*this);

    // Allocate a Play object.
    play_ptr_ = std::make_shared<Play>(scene_titles_);

    // Allocate Players object
    size_t num = override_flag ? minimum_players_ : MAX(minimum_players_, maximum_players_);

    for (size_t i = 0; i < num; ++i) {
        players_.push_back(std::make_shared<Player>(*play_ptr_));
    }

}

void Director::Cue() {
    visit_ops_t = kCUE; // Set vist operation to cue
    script_ptr_->accept(*this);
}

int Director::WaitForAllPartsDone() {
    const unsigned kWaitTimeMilliseconds = 100;
    // wait for every player done.
    try {
        while (!player_futures_.empty()) {
            for (auto it = player_futures_.begin(); it != player_futures_.end(); ++it) {
                std::future_status status;
                status = it->wait_for(std::chrono::milliseconds(kWaitTimeMilliseconds));
                if (status == std::future_status::ready) { // one task ready.
                    it->get(); // might cause a runtime exception.
                    player_futures_.erase(it);
                    break;
                }
            }
        }
    } catch (ProgramException& e) {
        // Invocate an interruption to those waiting threads.
        for_each(players_.begin(), players_.end(), std::mem_fn(&Player::InterruptCurrentPlay));
        return e.GetExceptionCode();
    }
    return 0;
}

void Director::Visit(SceneAST* node) {
    if (visit_ops_t == kSKIM) {
        SkimVisitScene(node);
    } else if (visit_ops_t == kCUE) {
        CueVisitScene(node);
    }
}

void Director::Visit(FragmentAST* node) {
    if (visit_ops_t == kSKIM) {
        SkimVisitFrag(node);
    } else if (visit_ops_t == kCUE) {
        CueVisitFrag(node);
    }
}

void Director::Visit(PartAST* node) {
    if (visit_ops_t == kSKIM) {
        // Passed
    } else if (visit_ops_t == kCUE) {
        CueVisitPart(node);
    }
}

void Director::SkimVisitScene(SceneAST* node) {
    scene_titles_.push_back(node->string_);

    // If this scene node has more than one fragment, e.g. n = 3,
    // Then insert n - 1 = 2 blank line into scene_titles
    for (size_t i = 0; i < node->Children_.size() - 1; ++i) {
        scene_titles_.push_back(std::string());
    }
}

void Director::SkimVisitFrag(FragmentAST* node) {
    size_t size = node->Children_.size();
    if (size > maximum_players_) {
        maximum_players_ = size;
    }
}

void Director::CueVisitScene(SceneAST*) {
    // pass, currently cue visit do nothing with scene.
}

void Director::CueVisitFrag(FragmentAST* node) {
    // Set required players number into the Play
    play_ptr_->SetNumPlayersForFrag(frag_counter++, node->Children_.size());
}

// Core code for cue excution.
void Director::CueVisitPart(PartAST* node) {
    // Get part information
    std::string part_name = node->string_;
    std::string part_filename = node->dialog_filename_;
    unsigned int part_frag_counter = frag_counter;
    
    // Select a Player
    std::shared_ptr<Player> player = Select();

    // Cue Palyer to enter (hand off Message)
    player_futures_.push_back(player->Enter(part_name, part_filename, part_frag_counter));
}

std::shared_ptr<Player> Director::Select() {
    return players_[player_idx_++ % players_.size()];
}