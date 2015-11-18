// DirectorProgram.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Director.h"

#include <iostream>
#include <sstream>

#include "utils.h"

#define ARG_NUM                     4
#define PROGRAM_POS                 0
#define SCRIPT_POS                  1
#define PLAYER_NUM_POS              2
#define OVERRIDE_POS                3
#define UNINITIALIZED_PLAYER_NUM    -1

bool HasPlayerNumArg(int const &argc);

int main(int argc, char* argv[])
{
    //test_play_player();
    // argc validation
    if (argc > ARG_NUM || argc <= SCRIPT_POS) {
        std::cout << "usage: " << argv[PROGRAM_POS] << " <script_file_name> " << "[<minimum number of players>]" << std::endl;
        return EXIT_FAILURE;
    }

    // Get script file name
    const std::string script_file_name = argv[SCRIPT_POS];

    // Check if user set it to override
    if (argc == ARG_NUM) {
        const std::string override_str = argv[OVERRIDE_POS];
        // Get the flag from input arguments, and store it in global vraiable override_flag.
        SetOverrideFlagFromArg(override_str);
    }

    // Get the path information from input arguments, and store in global variables path_string
    SetPathFromArg(script_file_name);

    // Get minimum player number if exist
    unsigned int minimum_player_num = UNINITIALIZED_PLAYER_NUM;
    if (HasPlayerNumArg(argc)) {
        std::stringstream iss(argv[PLAYER_NUM_POS]);
        iss >> minimum_player_num;
    }

    // Construct a Director pointer so that it can point to different Director's constructor
    Director *director = nullptr;
    try {
        if (minimum_player_num == UNINITIALIZED_PLAYER_NUM) { // player number doesn't set, use default
            director = new Director(script_file_name);
        }
        else {
            director = new Director(script_file_name, minimum_player_num);
        }
    }
    catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        if (director != nullptr) delete director;
        return EXIT_FAILURE;
    }

    // Do some other stuffs
    director->Cue();  // Since its HS/HA, director need only cue once.

    int error_code = director->WaitForAllPartsDone();

    delete director;

    return error_code;
}

bool HasPlayerNumArg(int const &argc) {
    return argc == ARG_NUM;
}

