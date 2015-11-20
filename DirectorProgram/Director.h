// Director.h -- Declares a Director class which read the Script tree and assigned role to Player and also
// make sure interrupt or wait for play progress
// Created by Yu Xiao, Yuhan Hao, Anqi Zhang
//
#ifndef DIRECTOR_H
#define DIRECTOR_H

#include "Play.h"
#include "Player.h"

#include <string>
#include <vector>
#include <memory>
#include "ASTNodes.h"
#include "Visitors.h"
#include <ace/Event_Handler.h>
#include "GenericFiniteStateMachine.h"

enum StateCode { kEntry, kIdle, kStart, kProgress, kStop, kQuit };
enum InputCode { inTimeout, inStart, inStop, inQuit };

class Director : 
        public ACE_Event_Handler, 
        public GenericFiniteStateMachine<StateCode, InputCode> {
    friend class DirectorSkimVisitor;
    friend class DirectorCueVisitor;
public:
    Director(std::vector<std::string> scripts_filename, unsigned minimum_players);

    ~Director();

    //
    // Director stops current play that performs in background
    //
    void Stop();

    void Start(unsigned idx);

    //
	// Cue()
	// Repeatedly hand off the name of a character and the name of a part definition file for that
	// character, and a scene fragment number, to a Player, and with the information they contain 
	// run the Player's read and act methods to perform that part within the play.
    //
	void Cue(unsigned play_idx);

    virtual int handle_timeout(const ACE_Time_Value& current_time, const void* act) override;
    virtual int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask) override;

    void SetSvcHandler(class RdWrSockSvcHandler*);

    void set_play_idx(unsigned idx);

protected:
    virtual void on_machine_setup() override;
private:
    //
    // Select()
    // Selects a player to assign the role.
    //
    std::shared_ptr<Player> Select();

    //
    // "Do-work" function in finite state machine
    RetCode OnEntryState(InputCode);
    RetCode OnIdleState(InputCode);
    RetCode OnStartState(InputCode);
    RetCode OnProgressState(InputCode);
    RetCode OnStopState(InputCode);
    RetCode OnQuitState(InputCode);
    
    // a container that holds the titles of the scenes (the same as in Play class)
    std::vector<std::string> scene_titles_;

    // a container of pointers that hold dynamicall allocated Play when this object is contructed.
    std::vector<std::shared_ptr<Play>> plays_;

    // a container to store the recruited Players
    std::vector<std::shared_ptr<Player>> players_;

    // a container of script objects to store the whole script tree
    std::vector<std::shared_ptr<ScriptAST>> scripts_;

    // an index used in thread (player) selection machanism.
    unsigned select_idx_;

    unsigned play_idx_;

    std::vector<std::future<bool>> player_futures_;

    RdWrSockSvcHandler* svc_handler_;
};

#endif
