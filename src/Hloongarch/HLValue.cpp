#include "../include/Hloongarch/HLValue.hpp"

std::string Reg::print() {
    if(is_fcc_){
        return Fccreg2name[id_];
    }
    else if(is_fp_) {
        return Freg2name[id_];
    } else {
        return Ireg2name[id_];
    }
}


std::string Reg::get_loongarch_code() {
    if(is_fcc_){
        return Fccreg2name[id_];
    }
    else if(is_fp_) {
        return Freg2name[id_];
    } else {
        return Ireg2name[id_];
    }
}

std::string Mem::print() {
    return "[" + Ireg2name[id_] + ", " +  std::to_string(offset_) + "]";
}


std::string Mem::get_loongarch_code() {
    return std::to_string(offset_) + "(" + Ireg2name[id_] + ")";
}

std::string Const::print() {
    if(is_fp_) {
        return std::to_string(fval_);
    } else {
        return std::to_string(ival_);
    }
}


std::string Const::get_loongarch_code() {
    if(is_fp_) {
        return std::to_string(*(uint32_t*)&(fval_));
    } else {
        return std::to_string(ival_);
    }
}