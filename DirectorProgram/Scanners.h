// Scanner.h -- Declares general Scanner, which contains most common operation for sub Scanner.
// Created by Yuhan Hao, Anqi Zhang, Yu Xiao
//
#ifndef SCANNERS_H
#define SCANNERS_H
#include <fstream>
#include "ASTNodes.h"
#include <regex>

class Scanner {
public:
	Scanner(std::ifstream& ifs, std::string path = std::string());

	virtual ~Scanner();

	operator bool() const;

	NodeAST::NodeType peek() const;

	std::shared_ptr<NodeAST> pop();

protected:
	virtual bool operator>> (std::shared_ptr<NodeAST>& node_ptr) = 0;

    std::string path_;
    std::ifstream& ifs_;
    std::shared_ptr<NodeAST> ptr_;
};

// class ScriptScanner
class ScriptScanner : public Scanner {
public:
    ScriptScanner(std::ifstream& ifs);

    virtual bool operator>> (std::shared_ptr<NodeAST>& container_ptr) override;

private:
    const std::regex SCENE_RE;

    const std::regex FRAG_RE;
};

// class PartScanner
class PartScanner : public Scanner {
public:
    PartScanner(std::ifstream& ifs, std::string& path);

private:
    virtual bool operator>> (std::shared_ptr<NodeAST>&) override;

    const std::regex CONFIG_RE;
};


#endif