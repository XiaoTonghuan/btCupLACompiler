#pragma once 

#include "PassManager.hpp"
#include "CFGAnalyse.hpp"
#include "Module.hpp"
#include <memory>

class LoopExpansion : public Pass
{
private:
    Module *module_;
    std::unique_ptr<CFGAnalyse> CFG_analyser;
    std::string name = "LoopExpansion";
public:
    explicit LoopExpansion(Module* module, bool print_ir=false): Pass(module, print_ir){module_=module;}
    void execute() final;
    void find_try();
    int loop_check(std::vector<BasicBlock*>* loop);
    int const_val_check(Instruction *inst);
    void expand(int time, std::vector<BasicBlock *>* loop);
    const std::string get_name() const override {return name;}
};