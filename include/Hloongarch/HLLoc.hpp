#pragma once

#include <string>

#include "Constant.hpp"

#include "HLValue.hpp"

class HLLoc {
public:
    virtual std::string print() = 0;
    virtual std::string get_loongarch_code() = 0;
};

class RegLoc: public HLLoc {
public:
    RegLoc(int id, bool is_fp): id_(id), is_fp_(is_fp) {}
public:
    std::string print();
    std::string get_loongarch_code();
    std::string get_reg_name();
    bool is_float() { return is_fp_; }
    int get_reg_id() { return id_; }

private:
    int id_;
    bool is_fp_;
};


class RegBase: public HLLoc {
public:
    RegBase(int id, int offset): id_(id), offset_(offset) {}

public:
    std::string print();
    std::string get_reg_name();
    std::string get_loongarch_code();
    int get_offset() { return offset_; }
    int get_reg_id() { return id_; }

private:
    int id_;
    int offset_;
};


class Label: public HLLoc {
public:
    Label(std::string label): label_(label) {}

public:
    std::string print();
    std::string get_loongarch_code();
    std::string get_label_name() { return label_; }

private:
    std::string label_;
};

class ConstPool: public HLLoc {
public:
    ConstPool(int const_int): const_int_(const_int), is_fp_(false) {}
    ConstPool(float const_fp): const_fp_(const_fp), is_fp_(true) {}
public:
    bool is_float() { return is_fp_; }
    std::string print();
    std::string get_loongarch_code();
    int &get_ival() { return const_int_; }
    float &get_fval() { return const_fp_; }

private:
    float const_fp_;
    int const_int_;
    bool is_fp_;
};