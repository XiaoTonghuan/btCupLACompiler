#include <cassert>

#include "User.hpp"

User::User(Type *ty, const std::string &name, unsigned num_ops) 
  : Value(ty, name), num_ops_(num_ops) {

    operands_.resize(num_ops_, nullptr);
}

Value *User::get_operand(unsigned i) const {
    return operands_[i];
}

void User::set_operand(unsigned i, Value *v) {
    assert(i < num_ops_ && "set_operand out of index");
    operands_[i] = v;
    v->add_use(this, i);
}

void User::add_operand(Value *v) {
    operands_.push_back(v);
    v->add_use(this, num_ops_);
    num_ops_++;
}

std::vector<Value*>& User::get_operands() {
    return operands_;
}

unsigned User::get_num_operands() const {
    return num_ops_;
}

void User::remove_operands(int index1, int index2) {
    for (int i = index1; i <= index2; i++) {
        operands_[i]->remove_use(this);
    }
    operands_.erase(operands_.begin() + index1, operands_.begin() + index2 + 1);
    num_ops_ = operands_.size();
}

void User::remove_use_of_ops() {
    for (auto op : operands_) {
        op->remove_use(this);
    }
}




/*#include "User.hpp"

#include <cassert>

void User::set_operand(unsigned i, Value *v) {
    assert(i < operands_.size() && "set_operand out of index");
    if (operands_[i]) { // old operand
        operands_[i]->remove_use(this, i);
    }
    if (v) { // new operand
        v->add_use(this, i);
    }
    operands_[i] = v;
}

void User::add_operand(Value *v) {
    assert(v != nullptr && "bad use: add_operand(nullptr)");
    v->add_use(this, operands_.size());
    operands_.push_back(v);
}

void User::remove_all_operands() {
    for (unsigned i = 0; i != operands_.size(); ++i) {
        if (operands_[i])
            operands_[i]->remove_use(this, i);
    }
    operands_.clear();
}

void User::remove_operand(unsigned idx) {
    assert(idx < operands_.size() && "remove_operand out of index");
    // influence on other operands
    for (unsigned i = idx + 1; i < operands_.size(); ++i) {
        operands_[i]->remove_use(this, i);
        operands_[i]->add_use(this, i - 1);
    }
    // remove the designated operand
    operands_[idx]->remove_use(this, idx);
    operands_.erase(operands_.begin() + idx);
}
*/