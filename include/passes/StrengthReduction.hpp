#pragma once

#include "Module.hpp"
#include "PassManager.hpp"
#include "Constant.hpp"
#include "ConstProp.hpp"
#include "IRBuilder.hpp"
#include "logging.hpp"
#include "utils.hpp"
#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <set>

class StrengthReduction : public Pass
{
private:
    Module *module_;
    std::string name = "StrengthReduction";

public:
    explicit StrengthReduction(Module* module, bool print_ir=false): Pass(module, print_ir){module_ = module;}
    ~StrengthReduction(){};
    void execute() final;
    const std::string get_name() const override {return name;}

    bool isNthPower(int x);
    void specInstReduct();
    void specInstReductStrict();
private:
    std::string filename = "";
};

