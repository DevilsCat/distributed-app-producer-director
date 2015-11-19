// Director.cpp -- Implements a Director class which read the Script tree and assigned role to Player and also
// make sure interrupt or wait for play progress
// Created by Yu Xiao, Yuhan Hao, Anqi Zhang
//
#include "stdafx.h"
#include "Director.h"
#include "ScriptParser.h"
#include <ace/Log_Msg.h>
#include "RdWrSockSvcHandler.h"

#define MAX(X,Y)    ((X) > (Y) ? (X) : (Y))
#undef GetExceptionCode

Director::Director(std::vector<std::string> script_filenames, unsigned min_nplayers) :
    min_nplayers_(min_nplayers), max_nplayers_(0), select_idx_(0), 
    svc_handler_(nullptr)
{
    // parse all files by given filenames
    for (std::string filename : script_filenames) {
        ScriptParser parser(filename);
        parser.Parse();
        scripts_.push_back(parser.script_ptr());
    }
    
    // Traverse all parsed scripts to Generate Play and maximum players needed.
    for (std::shared_ptr<ScriptAST> script : scripts_) {
        DirectorSkimVisitor visitor(script);  // After this visit, visitor will get all scene titles 
        script->accept(visitor);              // and number of players needed in each fragment.
        plays_.push_back(
            std::make_shared<Play>(visitor.scene_titles(), visitor.frag_nplayers())
        );
        max_nplayers_ = MAX(max_nplayers_, visitor.max_nplayers());  // Record the maximum number of
    }                                                                // players needed in all scripts

    // Generate maximum numbers of players.
    size_t greater = MAX(min_nplayers_, max_nplayers_);
    for (size_t i = 0; i < greater; ++i) {
        players_.push_back(std::make_shared<Player>());
    }
}

Director::~Director() {
    Stop();
}

void Director::Stop() {
    player_futures_.clear();
    for_each(players_.begin(), players_.end(), std::mem_fn(&Player::Exit));
}

void Director::Start(unsigned idx) {
    if (!available_state_) return;  // Not available means a play is on progress.
    for_each(players_.begin(), players_.end(), std::mem_fn(&Player::Activate));
    Cue(idx);
}

void Director::Cue(unsigned play_idx) {
    if (play_idx >= scripts_.size()) return;  // given index out of bound.
    
    // Set all Players an indexed play to act on.
    plays_[play_idx]->Reset();
    for (std::shared_ptr<Player> player : players_)
        player->SetPlay(plays_[play_idx].get());

    DirectorCueVisitor visitor(*this);
    scripts_[play_idx]->accept(visitor);

}

int Director::handle_timeout(const ACE_Time_Value& current_time, const void* act) {
    std::string available_str("Available");
    available_str += '\n';
    std::string inavailable_str("Inavailable");
    inavailable_str += '\n';
    if (player_futures_.empty()) {
        if (available_state_ == false) {
            available_state_ = true;
            svc_handler_->peer().send_n(available_str.c_str(), available_str.size());
        }
        return 0;
    }

    if (available_state_ == true) {
        available_state_ = false;
        svc_handler_->peer().send_n(inavailable_str.c_str(), inavailable_str.size());
    }
    
    const unsigned kWaitTimeMilliseconds = 1;
    std::vector<std::future<bool>>::iterator it = player_futures_.begin();
    try {
        while(it != player_futures_.end()) {
            std::future_status status = it->wait_for(std::chrono::milliseconds(kWaitTimeMilliseconds));
            if (status == std::future_status::ready) { // one task ready
                it->get();
                it = player_futures_.erase(it);
            } else {
                ++it;
            }
        }
    } catch (ProgramException& e) {
        ACE_DEBUG((LM_INFO, "%s", e.what()));
        Stop();
    }
    
    if (player_futures_.empty())  Stop();  // if normally finished, destroy all other threads.

    return 0;
}

int Director::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask) {
    delete this;
    return 0;
}

void Director::SetSvcHandler(RdWrSockSvcHandler* svc_handler) {
    svc_handler_ = svc_handler;
}

std::shared_ptr<Player> Director::Select() {
    return players_[select_idx_++ % players_.size()];
}
