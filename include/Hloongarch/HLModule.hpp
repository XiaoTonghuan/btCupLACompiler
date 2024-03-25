#pragma once

#include "Module.hpp"
#include "Function.hpp"

#include "HLFunction.hpp"

class HLFunction;
class HLBlock;


class HLModule {
public:
    explicit HLModule(Module *m): m_(m) {}
    void set_hloongarch_header(std::string hloongarch_header) { hloongarch_header_ = hloongarch_header; }
    void add_to_data_section(std::string global_var_def) { hloongarch_data_section_def_.push_back(global_var_def); } 
    Module *get_module() { return m_; }
    HLFunction *create_func(Function *func);

    std::string get_loongarch_code();
    std::string print();

private:
    Module *m_;
    std::list<HLFunction*> funcs_list_;
    std::string hloongarch_header_;
    std::vector<std::string> hloongarch_data_section_def_;
};