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
#include "ScriptParser.h"
#include "Visitor.h"

class Director : public Visitor {    
    enum VisitOps {
        kSKIM, kCUE
    };
public:
	//
	// Director constructor -- lab 2
	//
	Director(std::string const &script_file_string, unsigned int minimum_players);

	//
	// Director constructor -- lab 2
	// 
	Director(std::string const &script_file_string);

    virtual ~Director() override;

	//
	// Cue()
	// Repeatedly hand off the name of a character and the name of a part definition file for that
	// character, and a scene fragment number, to a Player, and with the information they contain 
	// run the Player's read and act methods to perform that part within the play.
    //
	void Cue();

    //
    // WaitForAllPartsDone()
    //
    int WaitForAllPartsDone();

    //
    // Visit()
    // Visits the SceneAST node, and stores each scene name
    //
    virtual void Visit(SceneAST* node) override;

    //
    // Visit()
    // Visit the FragmentAST node, and stores the maximum number of part children
    //
    virtual void Visit(FragmentAST* node) override;

    virtual void Visit(PartAST* node) override;

private:
    //
    // Different Script tree visiting mode.
    //
    void SkimVisitScene(SceneAST *);
    void SkimVisitFrag(FragmentAST *);
    void CueVisitScene(SceneAST *);
    void CueVisitFrag(FragmentAST *);
    void CueVisitPart(PartAST *);

    //
    // Select()
    // Selects a player to assign the role.
    //
    std::shared_ptr<Player> Select();

    VisitOps visit_ops_t;

    // A string for the name of a script file
    std::string script_file_string_;

    // A numeric for the minimum number of players to construct.
    size_t minimum_players_;

    // A numeric for the maximum number of players to construct.
    size_t maximum_players_;

    // a container that holds the titles of the scenes (the same as in Play class)
    std::vector<std::string> scene_titles_;

    // a smart pointer that hold dynamicall allocated Play when this object is contructed.
    std::shared_ptr<Play> play_ptr_;

    // a container to store the recruited Players
    std::vector<std::shared_ptr<Player>> players_;

    // a script object to store the whole script tree
    std::shared_ptr<ScriptAST> script_ptr_;

    ScriptParser parser_;

    unsigned int frag_counter;

    unsigned int player_idx_;

    std::vector<std::future<bool>> player_futures_;
};

#endif
