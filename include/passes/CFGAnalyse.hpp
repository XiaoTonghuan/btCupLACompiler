#pragma once

#include<vector>
#include<map>
#include<stack>
#include<set>

#include"Module.hpp"
#include"PassManager.hpp"

class CFGAnalyse : public Pass{
public:
    CFGAnalyse(Module *m, bool print_ir=false) : Pass(m, print_ir) {}
    ~CFGAnalyse() {}
    void execute() final;
    void incoming_find(Function* func);
    void incoming_DFS(BasicBlock* BB);
    void loop_find(Function* func);
    void tarjan_DFS(BasicBlock* BB);
    BasicBlock* find_loop_entry(std::vector<BasicBlock *>* loop){return *(*loop).rbegin();}
    
    //& Loopinvariant
    std::vector<std::vector<BasicBlock *> *>* get_loops() { return &loops; }
    std::vector<BasicBlock *>* find_bb_loop(BasicBlock* BB){return bb_loop[BB];}
    std::vector<BasicBlock *>* find_outer_loop(std::vector<BasicBlock*>* BB){return outer_loop[BB];}

    const std::string get_name() const override {return name;}
private:
    std::vector<std::vector<BasicBlock*>*> loops;
    std::map<BasicBlock *, std::vector<BasicBlock *> *> bb_loop;
    std::map<std::vector<BasicBlock *> *, std::vector<BasicBlock *> *> outer_loop;
    std::map<BasicBlock *, int> color;
    std::string name = "CFGAnalyse";
};