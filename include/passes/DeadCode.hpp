#pragma once

#include "FuncInfo.hpp"
#include "Function.hpp"
#include "Instruction.hpp"
#include "PassManager.hpp"
#include "logging.hpp"


/**
 * 死代码消除：参见 https://www.clear.rice.edu/comp512/Lectures/10Dead-Clean-SCCP.pdf
 **/
class DeadCode : public Pass {
public:
    DeadCode(Module *m, bool print_ir=false) : Pass(m, print_ir), func_info(std::make_shared<FuncInfo>(m)) {}
    void execute();

    const std::string get_name() const override {return name;}

private:
    std::shared_ptr<FuncInfo> func_info;
    void mark(Function *func);
    void mark(Instruction *ins);
    bool sweep(Function *func);
    bool is_critical(Instruction *ins);

private:
    //& 用以衡量死代码消除的性能
    int ins_count{0};
    const std::string name = "DeadCode";
    std::deque<Instruction *> work_list{};
    std::unordered_map<Instruction *, bool> marked{};
};
