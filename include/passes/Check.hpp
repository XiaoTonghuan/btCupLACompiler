#pragma once

#include "BasicBlock.hpp"
#include "Function.hpp"
#include "Instruction.hpp"
#include "Module.hpp"
#include "PassManager.hpp"
#include<algorithm>

class Check: public Pass {
public:
    explicit Check(Module *m, bool print_ir=false): Pass(m, print_ir) {}
    ~Check() {}
    void execute() final;
    const std::string get_name() const override {return name;}
    //& Check if Successors are Correct and Verify if Basic Block Ends with ret/br
    void SuccCheck();        
    //& Check if Predecessors are Correct
    void PreCheck();
    //& Check if Defined Value is Available before Reference
    void UsedefCheck();
    //& Check Def-Use Chain
    void DefuseCheck();
    //& Check if an instruction's parent is right
    void InstParentCheck();
private:
    const std::string name = "Check";
    bool FindError = false;
};


