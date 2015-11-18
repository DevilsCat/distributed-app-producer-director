// PrinterVisitor.h -- Declare a concrete printing operation on visiting AST.
// Created by Yu Xiao, Anqi Zhang, Yuhan Hao
//
#include "Visitor.h"

class PrintVisitor : public Visitor {
public:
    void Visit(struct ScriptAST* node) override;

    void Visit(struct SceneAST* node) override;

    void Visit(struct FragmentAST* node) override;

    void Visit(struct PartAST* node) override;

    void Visit(struct UnknownNodeAST* node) override;
};

