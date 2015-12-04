// ASTNodes.h -- This file declares AST nodes class family for script and play parsing tree.
// Created by Yu Xiao, Anqi Zhang, Yuhan Hao, all rights reserved.
//
#ifndef AST_NODES_H
#define AST_NODES_H

#include <vector>
#include <memory>

// struct NodeAST
struct NodeAST {
    //
    // ~NodeAST()
    // The virtual deconstrutor make this class as polymophism.
    //
    virtual ~NodeAST();

    //
    // enum NodeType
    // This enum declare the types of sub-node structs.
    //
    enum NodeType {
        SCRIPT = 0,
        SCENE,
        FRAGEMENT,
        PART,
        UNKNOWN
    };

    // stores the Node type for this object.
    NodeType type_;

    // stores the scanned string in this object.
    std::string string_;

    // contains many pointers of NodeAST as children nodes.
    std::vector<std::shared_ptr<NodeAST>> Children_;

    // The iterator used to iterate through the each children node
    std::vector<std::shared_ptr<NodeAST>>::iterator it_;

    //
    // Default Constructor
    // 
    NodeAST();

    //
    // Constructor
    //
    NodeAST(NodeType t, std::string& s);

    //
    // add_node()
    // Add a new children node (shared pointer) to this node.
    //
    void add_node(std::shared_ptr<NodeAST> node_ptr);

    //
    // accept()
    // Visitor Design Pattern which provides this tree with ability of accepting "visitors"
    // Noted that "visitor" should be implemented to define what to do in this level of node.
    //
    virtual void accept(class Visitor& v) = 0;
};

// struct ScriptAST
struct ScriptAST : NodeAST {
    //
    // Constructor
    // Takes the token string and labels itself as SCRIPT node type.
    //
    ScriptAST(std::string& s);

    void accept(Visitor& v) override;
};

// struct SceneAST
struct SceneAST : NodeAST {

    SceneAST(std::string&);

    void accept(Visitor& v) override;
};

// struct FragmentAST
struct FragmentAST : NodeAST {

    FragmentAST(std::string& s);

    void accept(Visitor& v) override;
};

// struct PartAST
struct PartAST : NodeAST {

    std::string dialog_filename_;

    PartAST(std::string& name, std::string& dialog_filename);

    void accept(Visitor& v) override;
};

// struct UnknownNodeAST
struct UnknownNodeAST : NodeAST {
    UnknownNodeAST();

    UnknownNodeAST(std::string& s);

    void accept(Visitor& v) override;
};

#endif
