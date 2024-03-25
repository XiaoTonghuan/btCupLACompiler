#include "../include/Hloongarch/HLLoc.hpp"
#include "../include/Hloongarch/HLValue.hpp"

std::string RegLoc::print() {
    if(is_fp_) {
        return Freg2name[id_];
    } else {
        return Ireg2name[id_];
    }
}

std::string RegLoc::get_loongarch_code() {
    if(is_fp_) {
        return Freg2name[id_];
    } else {
        return Ireg2name[id_];
    }
}

std::string RegBase::print() {
    return "[" + Ireg2name[id_] + ", " +  std::to_string(offset_) + "]";
}

std::string RegBase::get_loongarch_code() {
    return std::to_string(offset_) + "(" + Ireg2name[id_] + ")";
}

std::string Label::print() {
    return label_;;
}

std::string Label::get_loongarch_code() {
    return label_;
}

std::string ConstPool::print() {
    if(is_fp_) {
        return std::to_string(const_fp_);
    } else {
        return std::to_string(const_int_);
    }
}

std::string ConstPool::get_loongarch_code() {
    if(is_fp_) {
        return std::to_string(*(uint32_t*)&(const_fp_));
    } else {
        return std::to_string(const_int_);
    }
}