// PartParser.h -- Declares a parser for parsing Part file
// Created by Yu Xiao
//
#ifndef PART_PARSER_H
#define PART_PARSER_H
#include <fstream>
#include <memory>
#include "Scanners.h"
#include "ASTNodes.h"

class PartParser {
public:
	PartParser(std::ifstream&, const std::string& path);

	void Parse();
	
	bool HasNext() const;

	std::vector<std::shared_ptr<PartAST>>::iterator& GetIterator();

private:
	std::shared_ptr<PartAST> ParsePart();

    std::vector<std::shared_ptr<PartAST>> part_ptrs_;
    std::vector<std::shared_ptr<PartAST>>::iterator it_;
    std::ifstream ifs_;
    std::string path_;
    PartScanner part_scanner_;
};


#endif