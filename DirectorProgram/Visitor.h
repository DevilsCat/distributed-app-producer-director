// Visitor.h -- Declare an interface for visiting AST.
// Created by Yu Xiao, Anqi Zhang, Yuhan Hao
//
#ifndef VISITOR_H
#define VISITOR_H

class Visitor {
public:
    virtual ~Visitor() {}

    virtual void Visit(struct ScriptAST* node);

    virtual void Visit(struct SceneAST* node);

    virtual void Visit(struct FragmentAST* node);

    virtual void Visit(struct PartAST* node);

    virtual void Visit(struct UnknownNodeAST* node);
};


#endif
