#pragma once

#include "FuncInfo.hpp"
#include "Function.hpp"
#include "Instruction.hpp"
#include "PassManager.hpp"
#include "logging.hpp"
#include <unordered_set>
#include <map>


class DeadStoreEli : public Pass {
public:
    DeadStoreEli(Module *m, bool print_ir=false) : Pass(m, print_ir) {}
    void execute();

    const std::string get_name() const override {return name;}

private:
    void remove_dead_storeoffset(BasicBlock *bb);
    void remove_dead_store(BasicBlock *bb);
    void remove_dead_alloc_naive(Function *fun);
    const std::string name = "DeadStoreEli";

};
