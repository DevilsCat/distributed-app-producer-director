// Scanner.h -- Declares general Scanner, which contains most common operation for sub Scanner.
// Created by Yuhan Hao, Anqi Zhang, Yu Xiao
//
#ifndef SCANNER_H
#define SCANNER_H
#include <fstream>
#include "ASTNodes.h"

class Scanner {
public:
	Scanner(std::ifstream& ifs);

	virtual ~Scanner();

	operator bool() const;

	NodeAST::NodeType peek() const;

	std::shared_ptr<NodeAST> pop();

protected:
	virtual bool operator>> (std::shared_ptr<NodeAST>& node_ptr) = 0;

    std::ifstream& ifs_;

    std::shared_ptr<NodeAST> ptr_;
};

#endif