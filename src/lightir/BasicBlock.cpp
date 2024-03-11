#include <cassert>
#include <algorithm>

#include "BasicBlock.hpp"
#include "Function.hpp"
#include "IRprinter.hpp"

BasicBlock::BasicBlock(Module *m, const std::string &name = "", Function *parent = nullptr)
    : Value(Type::get_label_type(m), name), parent_(parent) {
    assert(parent && "currently parent should not be nullptr");
    parent_->add_basic_block(this);
}

BasicBlock *BasicBlock::create(Module *m, const std::string &name, Function *parent) {
    auto prefix = name.empty() ? "" : "label_";
    return new BasicBlock(m, prefix + name, parent);
}

Module *BasicBlock::get_module() {
    return get_parent()->get_parent();
}

const Instruction *BasicBlock::get_terminator() const {
    if (instr_list_.empty()) {
        return nullptr;
    }
    switch (instr_list_.back()->get_instr_type()) {
        case Instruction::ret: 
            return instr_list_.back();
        case Instruction::br: 
            return instr_list_.back();
        case Instruction::cmpbr:
            return instr_list_.back();
        case Instruction::fcmpbr:
            return instr_list_.back();
        default: 
            return nullptr;
    }
}

void BasicBlock::add_instruction(Instruction *instr) {
    instr_list_.push_back(instr);
}

void BasicBlock::add_instruction(std::list<Instruction*>::iterator instr_pos, Instruction *instr) {
    instr_list_.insert(instr_pos, instr);
}
void BasicBlock::add_instr_begin(Instruction *instr) {
    instr_list_.push_front(instr);
}

void BasicBlock::delete_instr(Instruction *instr) {
    instr_list_.remove(instr);
    instr->remove_use_of_ops();
}

std::list<Instruction*>::iterator BasicBlock::find_instruction(Instruction *instr) {
    return std::find(instr_list_.begin(), instr_list_.end(), instr);
}

void BasicBlock::erase_from_parent() { 
    this->get_parent()->remove_basic_block(this); 
}

std::string BasicBlock::print() {
    std::string bb_ir;
    bb_ir += this->get_name();
    bb_ir += ":";
    //// print prebb
    if (!this->get_pre_basic_blocks().empty()) {
        bb_ir += "                                                ; preds = ";
    }
    for (auto bb : this->get_pre_basic_blocks()) {
        if (bb != *this->get_pre_basic_blocks().begin())
            bb_ir += ", ";
        bb_ir += print_as_op(bb, false);
    }

    //// print prebb
    if (!this->get_parent()) {
        bb_ir += "\n";
        bb_ir += "; Error: Block without parent!";
    }
    bb_ir += "\n";
    for (auto &instr : this->get_instructions()) {
        bb_ir += "  ";
        bb_ir += instr->print();
        bb_ir += "\n";
    }

    return bb_ir;
}
/*#include "BasicBlock.hpp"
#include "Function.hpp"
#include "IRprinter.hpp"
#include "Module.hpp"

#include <cassert>

BasicBlock::BasicBlock(Module *m, const std::string &name = "",
                       Function *parent = nullptr)
    : Value(m->get_label_type(), name), parent_(parent) {
    assert(parent && "currently parent should not be nullptr");
    parent_->add_basic_block(this);
}

Module *BasicBlock::get_module() { return get_parent()->get_parent(); }
void BasicBlock::erase_from_parent() { this->get_parent()->remove(this); }

bool BasicBlock::is_terminated() const {
    if (instr_list_.empty())
        return false;
    switch (instr_list_.back().get_instr_type()) {
    case Instruction::ret:
    case Instruction::br:
        return true;
    default:
        return false;
    }
}

Instruction *BasicBlock::get_terminator() {
    assert(is_terminated() &&
           "Trying to get terminator from an bb which is not terminated");
    return &instr_list_.back();
}

void BasicBlock::add_instruction(Instruction *instr) {
    assert(not is_terminated() && "Inserting instruction to terminated bb");
    instr_list_.push_back(instr);
}

std::string BasicBlock::print() {
    std::string bb_ir;
    bb_ir += this->get_name();
    bb_ir += ":";
    // print prebb
    if (!this->get_pre_basic_blocks().empty()) {
        bb_ir += "                                                ; preds = ";
    }
    for (auto bb : this->get_pre_basic_blocks()) {
        if (bb != *this->get_pre_basic_blocks().begin())
            bb_ir += ", ";
        bb_ir += print_as_op(bb, false);
    }

    // print prebb
    if (!this->get_parent()) {
        bb_ir += "\n";
        bb_ir += "; Error: Block without parent!";
    }
    bb_ir += "\n";
    for (auto &instr : this->get_instructions()) {
        bb_ir += "  ";
        bb_ir += instr.print();
        bb_ir += "\n";
    }

    return bb_ir;
}*/
