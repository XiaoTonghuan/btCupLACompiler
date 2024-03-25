#pragma once

#include <string>

#include "../lightir/Function.hpp"

#include "HLModule.hpp"
#include "HLBlock.hpp"
#include "HLLoc.hpp"

class HLModule;
class HLBlock;

class HLFunction {
public:
    explicit HLFunction(HLModule *parent, Function *f): parent_(parent), f_(f) {}
    void set_func_header(std::string header) { func_header_ = header; }

    Function *get_function() { return f_; }

    HLBlock *create_bb(BasicBlock *bb, Label *label);

    std::string get_loongarch_code();
    std::string print();

private:
    std::string func_header_;
    std::list<HLBlock*> blocks_list_;
    HLModule *parent_;
    Function *f_;
};