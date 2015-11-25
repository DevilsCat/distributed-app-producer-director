// Visit.cpp -- Implements the Visitor for Visitor pattern, leave all behaivior as No-Ops.
// Created by Yuhan Hao, Anqi Zhang, Yu Xiao
//
#include "stdafx.h"
#include "Visitors.h"
#include "utils.h"
#include "ASTNodes.h"
#include "Director.h"
#include <iostream>

void Visitor::Visit(ScriptAST* node) {}

void Visitor::Visit(SceneAST* node) {}

void Visitor::Visit(FragmentAST* node) {}

void Visitor::Visit(PartAST* node) {}

void Visitor::Visit(UnknownNodeAST* node) {}

void Visitor::OnPostVisit(ScriptAST*) {}

void Visitor::OnPostVisit(SceneAST*) {}

void Visitor::OnPostVisit(FragmentAST*) {}

void Visitor::OnPostVisit(PartAST*) {}

void Visitor::OnPostVisit(UnknownNodeAST*) {}


// class PrintVistor
void PrintVisitor::Visit(ScriptAST* node) {
    std::cout << pad_whitespace(0) << "[script] " << node->string_ << std::endl;
}

void PrintVisitor::Visit(SceneAST* node) {
    std::cout << pad_whitespace(1) << "[scene] " << node->string_ << std::endl;
}

void PrintVisitor::Visit(FragmentAST* node) {
    std::cout << pad_whitespace(2) << "[frag] " << node->string_ << std::endl;
}

void PrintVisitor::Visit(PartAST* node) {
    std::cout << pad_whitespace(3) << "[part] " << node->string_ << " : " << node->dialog_filename_ << std::endl;
}

std::vector<std::string> DirectorSkimVisitor::scene_titles() const {
    return scene_titles_;
}

std::string DirectorSkimVisitor::play_name() const {
    return play_name_;
}

std::vector<unsigned> DirectorSkimVisitor::frag_nplayers() const {
    return frag_nplayers_;
}

unsigned DirectorSkimVisitor::max_nplayers() const {
    return *max_element(frag_nplayers_.begin(), frag_nplayers_.end());
}

void DirectorSkimVisitor::Visit(ScriptAST* node) {
    play_name_ = node->string_;
}

void DirectorSkimVisitor::Visit(SceneAST* node) {
    scene_titles_.push_back(node->string_);

    // If this scene node has more than one fragment, e.g. n = 3,
    // Then insert n - 1 = 2 blank line into scene_titles
    for (size_t i = 0; i < node->Children_.size() - 1; ++i) {
        scene_titles_.push_back(std::string());
    }
}

void DirectorSkimVisitor::Visit(FragmentAST* node) {
    frag_nplayers_.push_back(node->Children_.size());
}

DirectorCueVisitor::DirectorCueVisitor(Director& director) :
    director_(director), frag_counter_(0)
{}

void DirectorCueVisitor::Visit(FragmentAST* node) {
    frag_counter_++;
}

// Core code for cue excution.
void DirectorCueVisitor::Visit(PartAST* node) {
    // Get part information
    std::string part_name = node->string_;
    std::string part_filename = node->dialog_filename_;
    unsigned int part_frag_counter = frag_counter_;

    // Select & Cue Palyer to enter (hand off Message)
    std::shared_ptr<std::atomic<bool>> done_flag = director_.Select()->Enter(part_name, part_filename, part_frag_counter);
    //director_.player_futures_.push_back(move(uf));
    director_.player_done_flags_.push_back(done_flag);
}
