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
    GenericFiniteStateMachine<StateCode, InputCode>(kEntry),
    select_idx_(0)
{
    // parse all files by given filenames
    for (std::string filename : script_filenames) {
        ScriptParser parser(filename);
        parser.Parse();
        scripts_.push_back(parser.script_ptr());
    }
    
    size_t max_nplayers = 0;  // Stores maximum numbers that needed in all plays.
    // Traverse all parsed scripts to Generate Play and maximum players needed.
    for (std::shared_ptr<ScriptAST> script : scripts_) {
        DirectorSkimVisitor visitor;  // After this visit, visitor will get all scene titles 
        script->accept(visitor);              // and number of players needed in each fragment.
        plays_.push_back(
            std::make_shared<Play>(visitor.scene_titles(), visitor.frag_nplayers())
        );
        play_list_.push_back(visitor.play_name());
        max_nplayers = MAX(max_nplayers, visitor.max_nplayers());  // Record the maximum number of
    }                                                              // players needed in all scripts

    // Generate maximum numbers of players.
    size_t greater = MAX(min_nplayers, max_nplayers);
    for (size_t i = 0; i < greater; ++i) {
        players_.push_back(std::make_shared<Player>());
    }

    // Setup Finite State Machine
    GenericFiniteStateMachine<StateCode, InputCode>::setup();
}

Director::~Director() {
    Stop();  // Guard this objects, stops all the background player anyway.
}

void Director::Stop() {
    player_futures_.clear();
    for_each(players_.begin(), players_.end(), std::mem_fn(&Player::Exit));
}

void Director::Start(unsigned idx) {
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
    run(inTimeout);
    return 0;
}

int Director::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask) {
    delete this;
    return 0;
}

void Director::set_play_idx(unsigned idx) {
    play_idx_ = idx;
}

std::shared_ptr<Player> Director::Select() {
    return players_[select_idx_++ % players_.size()];
}

GenericFiniteStateMachine<StateCode, InputCode>::RetCode Director::OnEntryState(InputCode input) {
    ACE_DEBUG((LM_INFO, "OnEntryState:%d -> ok\n", input));
    SockMsgHandler::instance()->FeedbackPlayList(play_list_);  // notice the producer by sending a socket.
    return kOk;
}

GenericFiniteStateMachine<StateCode, InputCode>::RetCode Director::OnIdleState(InputCode input) {
    if (input == inQuit)  return kFail;
    if (input == inStart) return kOk;
    return kRepeat;
}

GenericFiniteStateMachine<StateCode, InputCode>::RetCode Director::OnStartState(InputCode) {
    ACE_DEBUG((LM_INFO, "OnStartState -> ok\n"));
    Start(play_idx_);
    SockMsgHandler::instance()->FeedbackStatus(false, play_idx_);
    return kOk;
}

GenericFiniteStateMachine<StateCode, InputCode>::RetCode Director::OnProgressState(InputCode input) {
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
        return kOk;
    }
    
    if (player_futures_.empty() || input == inStop)
        return kOk;
    if (input == inQuit)
        return kFail;
    return kRepeat;
}

GenericFiniteStateMachine<StateCode, InputCode>::RetCode Director::OnStopState(InputCode input) {
    ACE_DEBUG((LM_INFO, "OnStopState -> ok\n"));
    Stop();
    SockMsgHandler::instance()->FeedbackStatus(true);
    return kOk;
}

GenericFiniteStateMachine<StateCode, InputCode>::RetCode Director::OnQuitState(InputCode input) {
    ACE_DEBUG((LM_INFO, "OnQuitState -> ok\n"));
    Stop();
    ACE_Reactor::instance()->end_event_loop();
    return kOk;
}

void Director::on_machine_setup() {
    ACE_DEBUG((LM_INFO, "On State Machine Setup\n"));

    // Add state working functions
    add_state_function(kEntry, std::bind(&Director::OnEntryState, this, std::placeholders::_1));
    add_state_function(kIdle, std::bind(&Director::OnIdleState, this, std::placeholders::_1));
    add_state_function(kStart, std::bind(&Director::OnStartState, this, std::placeholders::_1));
    add_state_function(kProgress, std::bind(&Director::OnProgressState, this, std::placeholders::_1));
    add_state_function(kStop, std::bind(&Director::OnStopState, this, std::placeholders::_1));
    add_state_function(kQuit, std::bind(&Director::OnQuitState, this, std::placeholders::_1));

    // Add transition rules
    add_transition(kEntry, kOk, kIdle);
    add_transition(kEntry, kFail, kQuit);
    add_transition(kIdle, kOk, kStart);
    add_transition(kIdle, kFail, kQuit);
    add_transition(kIdle, kRepeat, kIdle);
    add_transition(kStart, kOk, kProgress);
    add_transition(kProgress, kOk, kStop);
    add_transition(kProgress, kFail, kQuit);
    add_transition(kProgress, kRepeat, kProgress);
    add_transition(kStop, kOk, kIdle);
}
