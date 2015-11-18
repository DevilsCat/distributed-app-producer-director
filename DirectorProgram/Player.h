// Player.h -- Declares a Player class, which will performs multiple roles that director assigned to them.
// Created by Anqi Zhang, Yu Xiao, Yuhan Hao
//
#ifndef PLAYER_H
#define PLAYER_H

#include "stdafx.h"
#include "Play.h"
#include "Active_Object.h"
#include "Message.h"
#include <future>

class Player: ActiveObject<bool> {
    //
    // enum MessageType
    // Used to Declare the protocal of Message used between Invocator (Director Thread)
    // Executor (Background worker thread).
    //
    enum MessageType {
        ACT = 0,
        EXIT
    };
    const std::string sName = "NAME";
    const std::string sFileName = "FILE";
    const std::string sFrag = "FRAG";
public:
    virtual ~Player();

    //
	// Public constructor
	// Only keeps track of a reference to the Play in which it is performing
	//
	Player(Play& p);

	//
	// Read()
	// Reads role lines from a given file stream.
	//
	void Read(std::ifstream &in);

	//
	// Act()
	// Repeatedly pull from the object's input queue and process each item it is given.
    //
	void Act(const std::string& name, const unsigned& frag_number);

	//
	// Enter()
	// Interface for sending Role task to worker thread. Returns the future for given task.
    // Director should via this method assign role to each thread.
	//
	std::future<bool> Enter(const play_t::name_t& name, const std::string& file_name, unsigned frag_number);

	//
    // Service()
    // Implements concrete Service that need to run in the background thread.
    // Indeed this performs Act or Exit.
    //
	virtual void Service(std::shared_ptr<Message<bool>>) override;

	//
	// Exit()
	// Sends a termination Message to the background thread and wait for it exits.
	//
	void Exit();

    //
    // InterruptCurrentPlay()
    // Interrupts the current play that background Player is acting on.
    //
    void InterruptCurrentPlay();

	//
	// empty()
	// Return true if nothing is inside the dialog map.
	//
	bool empty() const;

private:
    //
    // ReadLine()
    // Reads each line from the actor's line (ifs) and stores into map.
    //
    static void ReadLine(unsigned int, std::string &, Player&);

    // Move the container of structured lines from the Play class into it as a private member variable.
    std::map<play_t::number_t, play_t::dialog_t> play_map_;

    // Private member variables associate with constructor
    Play* p_;

    play_t::name_t name_;

};

#endif