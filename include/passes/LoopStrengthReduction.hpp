#pragma once

#include "DeadCodeEliWithBr.hpp"
#include "Module.hpp"
#include "PassManager.hpp"
#include "Constant.hpp"
#include "ConstProp.hpp"
#include "IRBuilder.hpp"
#include "logging.hpp"
#include "utils.hpp"
#include "LoopInfo.hpp"
#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <set>

class LoopStrengthReduction : public Pass
{
private:
    Module *module_;
    std::string name = "LoopStrengthReduction";

public:
    explicit LoopStrengthReduction(Module* module, bool print_ir=false): Pass(module, print_ir){module_ = module;}
    ~LoopStrengthReduction(){};
    void execute() final;
    const std::string get_name() const override {return name;}


private:
    LoopInfo *loopInfo;
    std::vector<Loop *> *loopSet;
    std::string filename = "";

    void init_loop_info();
    void strength_reduction();
};

