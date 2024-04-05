#include "../include/Hloongarch/HLFunction.hpp"
#include "../include/Hloongarch/HLValue.hpp"
#include "../include/Hloongarch/H2L.hpp"

#include "logging.hpp"

HLBlock *HLFunction::create_bb(BasicBlock *bb, Label *label) { 
    auto new_bb = new HLBlock(this, bb, label);
    blocks_list_.push_back(new_bb);
    return new_bb;
}

std::string HLFunction::get_loongarch_code() {
    std::string loongarch_code;
    //std::cout<<"HLFunction start"<<this->print()<<std::endl;
    loongarch_code += func_header_;
    loongarch_code += f_->get_name() + ":" + H2L::newline;
    for(auto bb: blocks_list_) {
        loongarch_code += bb->get_loongarch_code();
    }
    loongarch_code += H2L::space + ".size" + H2L::space + f_->get_name() + ", " + ".-" + f_->get_name() + H2L::newline;
    //std::cout<<"HLFunction end"<<std::endl;
    return loongarch_code;
}


std::string HLFunction::print() {
    std::string hloongarch_code;
    hloongarch_code += func_header_;
    hloongarch_code += f_->get_name() + ":" + H2L::newline;
    for(auto bb: blocks_list_) {
        hloongarch_code += bb->print();
    }
    hloongarch_code += H2L::space + ".size" + H2L::space + f_->get_name() + ", " + ".-" + f_->get_name() + H2L::newline;
    return hloongarch_code;
}