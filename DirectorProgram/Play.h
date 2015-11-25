// Play.h -- This file declares a Play class, which allows players to enter and "play".
// for all "Players" to have dialogs in order.
// Created by Anqi Zhang, Yu Xiao, Yuhan Hao
//
#ifndef PLAY_H
#define PLAY_H

#include "stdafx.h"
#include "play_t.h"
#include <mutex>
#include <condition_variable>
#include <map>
#include <vector>

class Play {
    enum PlayProgress {
        PLAY_BEGIN = 0,
        FRAG_SWITCH,
        SCENE_SWITCH,
        PLAY_END
    };
    
public:
    //
    // Play constructor -- lab 2 
    // Takes and stores a reference to a container of strings, initialize the container iterator
    //
    Play(const std::vector<std::string> &scene_title_strings,
         const std::vector<unsigned>& required_pnum_v);

    //
    // Recite()
    // Takes a reference to an iterator over a container of structured lines 
    // Also takes a numeric argument given the number of the current scene fragment in which the lines are to
    // be recited.
    //
    // This method is called by Player, to perform "recite" on this play -- if is this Player's turn, it will
    // print out Player's current content and his Role Name (if Role switch happens).
    //
    void Recite(std::map<play_t::number_t, play_t::dialog_t>::iterator &it, const unsigned & frag_counter);

    //
    // Enter()
    // Takes a numeric argument for the scene fragment that Player is attempting to Enter. 
    //
    // This method is called by Player, to perform "entering" this Play -- if this Player turns out to 
    // be inside the play, he can do the following EnterStage.
    //
    void Enter(const unsigned & scene_frag_i);

    //
    // EnterStage()
    // Takes the the name of role who intends to enter the stage.
    // 
    // This method is called by Player, to perform "entering" the stage -- Only when Player is on the
    // stage, then he can performs "reciting".
    //
    // Player who calls this method should wait all other player to be on the stage, and the last one
    // that enters, should act as an "Announcer" to AnnouncePlayStatus -- who entered, exited or scene
    // title changed.
    //
    void EnterStage(const std::string& name);

    //
    // Exit()
    // Takes the name of role who intends to exit the stage (This guy should finished recite all his contents).
    //
    // This method is called by Player, to perform "exiting" the stage.
    //
    // This method tracks numbers and who is leaving the stage, when last one on the stage is about to leave, 
    // he need increase the progress information to represent this fragement is end and another one is about
    // to begin.
    //
    void Exit();

    //
    // Reset()
    // Resets this play in order to play again.
    void Reset();

private:
    //
    // MoveToNextFrag()
    // Modifies Play progress, so that it moves to next fragment.
    //
    void MoveToNextFrag();

    //
    // AnnouncePlayProgress()
    // Announces the progress of play, including who entered, who exited and
    // what's the scene title.
    //
    void AnnouncePlayProgress();

    //
    // Announce[Family]_()
    // All of these are helper for AnnouncePlayerProgress_() or if not
    // Annouce to screen the information of Play Progress.
    //
    void AnnounceRoleSwitch();
    void AnnounceEnter(const std::map<std::string, bool> &who);
    void AnnounceExit(const std::map<std::string, bool> &who);
    void AnnounceEnter(const std::string&);
    void AnnounceExit(const std::string&);
    void AnnounceScene(const std::string&);
    void AnnonceRoleName(const std::string&);
    
    // A container to store all scene titles
    std::vector<std::string> scene_titles_;

    // An iterator points to the current scene title in the container.
    std::vector<std::string>::const_iterator scene_title_iterator_;

    // applying the scoped locking or thread safe interface patterns
    std::mutex recite_m_;
    std::mutex enter_m_;
    std::mutex exit_m_;
    std::mutex role_switch_m_;

    // applying a conditional variable to avoid race condition
    std::condition_variable recite_cond_;
    std::condition_variable stage_cond_;
    std::condition_variable role_switch_cond_;

    // lab 1 -- current player in the play, used to determine who is the one last speaking
    std::string last_one_acting_;

    // lab 2 -- line_counter_
    unsigned int line_counter_;

    // lab 2 -- member variable for the current scene fragment that is in progress (0 or 1 ?)
    unsigned int frag_counter_;

    // lab 2 -- a numeric member variable for the number of characters currently on stage in the current scene fragment
    unsigned int on_stage_;

    // Records who (previously) is on the stage. This should be called when Player Enter and Exit the Play.
    std::map<std::string, bool> prev_who_on_stage;
    std::map<std::string, bool> now_who_on_stage;

    // Records how many player required repected to it fragment number [index]
    std::vector<unsigned int> required_pnum_v_;

    // Maintains the Progress of Play.
    PlayProgress progress_;
};

#endif