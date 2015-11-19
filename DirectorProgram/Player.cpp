// Player.h -- Implements a Player class, which will performs multiple roles that director assigned to them.
// Created by Anqi Zhang, Yu Xiao, Yuhan Hao
//
#include "stdafx.h"
#include "Player.h"
#include "utils.h"
#include <sstream>
#include <fstream>
#include "ProgramException.h"

Player::~Player() {}

Player::Player() :
	p_(nullptr) 
{}

void Player::SetPlay(Play* p) { p_ = p; }

void Player::Read(std::ifstream &in) {
	play_map_.clear();

    int ret = ParseFile<Player>(in, *this, ReadLine);
    if (ret) {
        throw ProgramException(
            "Open File Failed", 
            ProgramException::ExceptionType(ret)
        );
    }
}

bool Player::empty() const {
	return play_map_.empty();
}

void Player::Act(const std::string& name, const unsigned& frag_number) {
	p_->Enter(frag_number);

    p_->EnterStage(name);

	std::map<play_t::number_t, play_t::dialog_t>::iterator it = play_map_.begin();
	for (; it != play_map_.end(); ++it) {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));  // make program running slowly for debugging.
		p_->Recite(it, frag_number);
	}

	p_->Exit();
}

// Player Enter with all the info in a message
// enqueue the message to the active object's queue.
// set the future so that the caller can know when it's done.
std::future<bool> Player::Enter(const play_t::name_t& name, const std::string& file_name, unsigned frag_number) {
    std::shared_ptr<Message<bool>> msg = Message<bool>::MakeMessage(ACT);

    msg->SetStrMessage(sName, name);
    msg->SetStrMessage(sFileName, file_name);
    msg->SetUnsignedMessage(sFrag, frag_number);

	std::future<bool> f = msg->GetFuture();
	enqueue(*msg);
	return f;
}

// override Service, read, act 
void Player::Service(std::shared_ptr<Message<bool>> msg_ptr) {
    switch (msg_ptr->What()) {
    case EXIT: {
        msg_ptr->SetPromiseValueAtThreadExit(true);
        Deactivate();
        break;
    }
    case ACT: {
        // retrieve the argument from message
        const std::string name = msg_ptr->GetStrMessage(sName);
        const std::string filename = msg_ptr->GetStrMessage(sFileName);
        const unsigned frag_number = msg_ptr->GetUnsignedMessage(sFrag);

        std::ifstream in_(filename);
        name_ = name;

        Read(in_);
        Act(name, frag_number);
        msg_ptr->SetPromiseValue(true);
        
        break;
    }
    default: 
        break;
    }
	
}

void Player::Exit() {
    if (!IsActive()) return;  // No need to invoke exit, since the background thread is inactive.

    InterruptService();  // be interrupted when it's in progress
    
    std::shared_ptr<Message<bool>> msg = Message<bool>::MakeMessage(EXIT);
    std::future<bool> uf = msg->GetFuture();
    enqueue(*msg);

    uf.get();  // Block and wait until it quit.
}

void Player::ReadLine(unsigned int idx, std::string &line, Player& p) {
    std::vector<play_t::number_t> nums;  // Declare the data we wanna store
    std::vector<play_t::dialog_t> diags;

    std::istringstream inputString(line);
    unsigned int lineNumber = 0;
    std::string content;
    inputString >> lineNumber >> std::ws;  // Since the rest line always contain a white space
    std::stringstream tmp;
    getline(inputString, content);
    if (!content.empty()) {
        play_t::dialog_t temp;
        temp.first = p.name_;
        temp.second = content;
        std::pair<play_t::number_t, play_t::dialog_t> res = std::make_pair(lineNumber, temp);
        p.play_map_.insert(res);
    }
}