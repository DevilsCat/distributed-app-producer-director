// ScriptParser.h -- Declares a parser for parsing Script file
// Created by Yu Xiao
//
#ifndef SCRIPT_PARSER_H
#define SCRIPT_PARSER_H

#include <memory>
#include "Scanners.h"

class ScriptParser {
public:
	ScriptParser(std::string&);

	void Parse();

    std::shared_ptr<ScriptAST> script_ptr();

	void PrintScript(); // Only For test.

private:
    // Concrete Parsing function base on BNF.
	std::shared_ptr<ScriptAST> ParseScript();
	std::shared_ptr<SceneAST> ParseScene();
	std::shared_ptr<FragmentAST> ParseFragement();

    std::shared_ptr<ScriptAST> script_ptr_;

    std::string filename_;

    std::ifstream ifs_;

    ScriptScanner scanner_;
};

#endif