﻿#include "stdafx.h"
#include "ASTNodes.h"
#include "Visitor.h"

NodeAST::~NodeAST()
{}

NodeAST::NodeAST() :
type_(UNKNOWN), it_(Children_.begin())
{}

NodeAST::NodeAST(NodeType t, std::string& s) :
type_(t), string_(s), it_(Children_.begin())
{}

void NodeAST::add_node(std::shared_ptr<NodeAST> node_ptr) {
    Children_.push_back(node_ptr);
}

// ScriptAST
ScriptAST::ScriptAST(std::string& s) : NodeAST(SCRIPT, s) {}

void ScriptAST::accept(Visitor& v) {
    v.Visit(this);

    std::vector<std::shared_ptr<NodeAST>>::const_iterator it = Children_.begin();
    for (; it != Children_.end(); ++it) {
        std::dynamic_pointer_cast<SceneAST>(*it)->accept(v);
    }
}

// SceneAST
SceneAST::SceneAST(std::string& s) :
NodeAST(SCENE, s)
{}

void SceneAST::accept(Visitor& v) {
    v.Visit(this);
    std::vector<std::shared_ptr<NodeAST>>::const_iterator it = Children_.begin();
    for (; it != Children_.end(); ++it) {
        std::dynamic_pointer_cast<FragmentAST>(*it)->accept(v);
    }
}

// FragmentAST
FragmentAST::FragmentAST(std::string& s) :
NodeAST(FRAGEMENT, s)
{}

void FragmentAST::accept(Visitor& v) {
    v.Visit(this);
    std::vector<std::shared_ptr<NodeAST>>::const_iterator it = Children_.begin();
    for (; it != Children_.end(); ++it) {
        std::dynamic_pointer_cast<PartAST>(*it)->accept(v);
    }
}

// PartAST
PartAST::PartAST(std::string& name, std::string& dialog_filename) :
NodeAST(PART, name), dialog_filename_(dialog_filename)
{}

void PartAST::accept(Visitor& v) {
    v.Visit(this);
}

// UnknownNodeAST
UnknownNodeAST::UnknownNodeAST() :
NodeAST()
{}

UnknownNodeAST::UnknownNodeAST(std::string& s) :
NodeAST(UNKNOWN, s)
{}

void UnknownNodeAST::accept(Visitor& v) {
    v.Visit(this);
}
