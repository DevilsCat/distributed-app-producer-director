// PartScanner.h -- Declare a scanner to scan line from Part file.
// Create by Yu Xiao
//
#ifndef PART_SCANNER_H
#define PART_SCANNER_H
#include <fstream>
#include <memory>
#include <regex>
#include "Scanner.h"
#include "ASTNodes.h"

class PartScanner : public Scanner {
public:
	PartScanner(std::ifstream& ifs);

private:
	virtual bool operator>> (std::shared_ptr<NodeAST>&) override;

    const std::regex CONFIG_RE;
};


#endif