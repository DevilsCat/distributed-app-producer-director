// Visit.cpp -- Implements the Visitor for Visitor pattern, leave all behaivior as No-Ops.
// Created by Yuhan Hao, Anqi Zhang, Yu Xiao
//
#include "stdafx.h"
#include "Visitor.h"

void Visitor::Visit(ScriptAST* node) {}

void Visitor::Visit(SceneAST* node) {}

void Visitor::Visit(FragmentAST* node) {}

void Visitor::Visit(PartAST* node) {}

void Visitor::Visit(UnknownNodeAST* node) {}