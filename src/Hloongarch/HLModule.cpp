#include "../include/Hloongarch/HLModule.hpp"
#include "logging.hpp"
#include "../include/Hloongarch/HLValue.hpp"
#include "../include/Hloongarch/H2L.hpp"

HLFunction *HLModule::create_func(Function *func) {
    auto new_func = new HLFunction(this, func);
    funcs_list_.push_back(new_func);
    return new_func;
}

std::string HLModule::get_loongarch_code() {
    std::string loongarch_code;

    loongarch_code += hloongarch_header_;
    if(!hloongarch_data_section_def_.empty()) {
        loongarch_code += H2L::space + ".text" + H2L::newline;
        for(auto data_def: hloongarch_data_section_def_) {
            loongarch_code += data_def;
        }
    }
    if(!funcs_list_.empty()) {
        loongarch_code += H2L::space + ".text" + H2L::newline;
        for(auto func: funcs_list_) {
            loongarch_code += func->get_loongarch_code();
        }
    }
    return loongarch_code;
}


std::string HLModule::print() {
    std::string hloongarch_code;

    hloongarch_code += hloongarch_header_;
    if(!hloongarch_data_section_def_.empty()) {
        hloongarch_code += H2L::space + ".text" + H2L::newline;
        for(auto data_def: hloongarch_data_section_def_) {
            hloongarch_code += data_def;
        }
    }
    if(!funcs_list_.empty()) {
        hloongarch_code += H2L::space + ".text" + H2L::newline;
        for(auto func: funcs_list_) {
            hloongarch_code += func->print();
        }
    }
    return hloongarch_code;
}