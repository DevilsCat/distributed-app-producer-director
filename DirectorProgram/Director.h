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

class Director {
    friend class DirectorSkimVisitor;
    friend class DirectorCueVisitor;
public:
    Director(std::vector<std::string> scripts_filename, unsigned minimum_players);

    ~Director();

	//
	// Cue()
	// Repeatedly hand off the name of a character and the name of a part definition file for that
	// character, and a scene fragment number, to a Player, and with the information they contain 
	// run the Player's read and act methods to perform that part within the play.
    //
	void Cue(unsigned play_idx);

    //
    // WaitForAllPartsDone()
    //
    int WaitForAllPartsDone();

private:
    //
    // Select()
    // Selects a player to assign the role.
    //
    std::shared_ptr<Player> Select();

    // A string for the name of a script file
    std::string script_file_string_;

    // A numeric for the minimum number of players to construct.
    size_t minimum_players_;

    // A numeric for the maximum number of players to construct.
    size_t maximum_players_;

    // a container that holds the titles of the scenes (the same as in Play class)
    std::vector<std::string> scene_titles_;

    // a container of pointers that hold dynamicall allocated Play when this object is contructed.
    std::shared_ptr<Play> play_;
    std::vector<std::shared_ptr<Play>> plays_;

    // a container to store the recruited Players
    std::vector<std::shared_ptr<Player>> players_;

    // a container of script objects to store the whole script tree
    std::vector<std::shared_ptr<ScriptAST>> scripts_;

    unsigned int frag_counter;

    unsigned int player_idx_;

    std::vector<std::future<bool>> player_futures_;
};

#endif
