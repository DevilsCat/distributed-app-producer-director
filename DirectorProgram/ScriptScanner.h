// ScriptScanner.h -- Declares a scanner to scan line from Script file.
// Create by Yu Xiao
//
#ifndef SCRIPT_SCANNER
#define SCRIPT_SCANNER

#include <memory>
#include <regex>

#include "Scanner.h"

class ScriptScanner : public Scanner {
public:
	ScriptScanner(std::ifstream& ifs);

	virtual bool operator>> (std::shared_ptr<NodeAST>& container_ptr) override;

private:
    const std::regex SCENE_RE;

    const std::regex FRAG_RE;
};

#endif