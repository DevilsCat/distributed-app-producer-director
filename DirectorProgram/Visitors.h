// Visitor.h -- Declare an interface for visiting AST.
// Created by Yu Xiao, Anqi Zhang, Yuhan Hao
//
#ifndef VISITORS_H
#define VISITORS_H
#include <vector>
#include <memory>

class Visitor {
public:
    virtual ~Visitor() {}

    virtual void Visit(struct ScriptAST* node);
    virtual void Visit(struct SceneAST* node);
    virtual void Visit(struct FragmentAST* node);
    virtual void Visit(struct PartAST* node);
    virtual void Visit(struct UnknownNodeAST* node);

    virtual void OnPostVisit(ScriptAST*);
    virtual void OnPostVisit(SceneAST*);
    virtual void OnPostVisit(FragmentAST*);
    virtual void OnPostVisit(PartAST*);
    virtual void OnPostVisit(UnknownNodeAST*);
};

class PrintVisitor : public Visitor {
public:
    void Visit(struct ScriptAST* node) override;
    void Visit(struct SceneAST* node) override;
    void Visit(struct FragmentAST* node) override;
    void Visit(struct PartAST* node) override;
};

class DirectorSkimVisitor : public Visitor {
public:
    DirectorSkimVisitor(std::shared_ptr<ScriptAST>&);
    std::vector<std::string> scene_titles() const;
    std::vector<unsigned> frag_nplayers() const;
    unsigned max_nplayers() const;

    virtual void Visit(SceneAST* node) override;
    virtual void Visit(FragmentAST* node) override;
private:
    std::shared_ptr<ScriptAST> script_;
    std::vector<std::string> scene_titles_;
    std::vector<unsigned> frag_nplayers_;
};

class DirectorCueVisitor : public Visitor {
public:
    DirectorCueVisitor(class Director&);

    virtual void Visit(FragmentAST* node) override;
    virtual void Visit(PartAST* node) override;
private:
    Director& director_;
    unsigned frag_counter_;
};

#endif
