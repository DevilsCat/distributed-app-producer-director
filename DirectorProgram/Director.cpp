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
Director::Director(std::vector<std::string> script_filenames, unsigned minimum_players) :
    minimum_players_(minimum_players),
    maximum_players_(0),
    frag_counter(0),
    player_idx_(0),
    svc_handler_(nullptr)
{
    // parse all files by given filenames
    for (std::string filename : script_filenames) {
        ScriptParser parser(filename);
        parser.Parse();
        scripts_.push_back(parser.script_ptr());
    }
    
    // Traverse all parsed scripts to Generate Play and maximum players needed.
    for (size_t i = 0; i < scripts_.size(); ++i) {
        DirectorSkimVisitor visitor(scripts_[i]);
        scripts_[i]->accept(visitor);
        plays_.push_back(
            std::make_shared<Play>(visitor.scene_titles(), visitor.frag_nplayers())
        );
        maximum_players_ = MAX(maximum_players_, visitor.max_nplayers());
    }

    size_t num = MAX(minimum_players_, maximum_players_);
    for (size_t i = 0; i < num; ++i) {
        players_.push_back(std::make_shared<Player>());
    }
}

Director::~Director() {
    Stop();
    //try {
    //    // wait all parts to be finished, interrupts
    //    // threads if any exception is thrown.
    //    WaitForAllPartsDone();
    //}
    //catch (ProgramException&) {
    //    // Invocate an interruption to those waiting threads.
    //    for_each(players_.begin(), players_.end(), std::mem_fn(&Player::InterruptCurrentPlay));
    //}

    //// Invocate a terminating signal to those "idle" waiting threads.
    //for_each(players_.begin(), players_.end(), std::mem_fn(&Player::Exit));

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

int Director::handle_timeout(const ACE_Time_Value& current_time, const void* act) {
    std::string available_str("Available");
    available_str += '\n';
    std::string inavailable_str("Inavailable");
    inavailable_str += '\n';
    if (player_futures_.empty()) {
        if (available_state_ == false) {
            Stop();
            available_state_ = true;
            svc_handler_->peer().send_n(available_str.c_str(), available_str.size());
        }
        //ACE_DEBUG((LM_INFO, "Director Available\n"));
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
    return players_[player_idx_++ % players_.size()];
}
