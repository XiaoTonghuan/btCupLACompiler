#pragma once

#include <string>

//& 定义RV寄存器的别名 
const int reg_zero = 0;      //~ 硬编码0
const int reg_ra = 1;        //~ 返回地址
const int reg_tp = 2;        //~ 线程指针
const int reg_sp = 3;        //~ 栈顶指针
const int reg_a0 = 4;        //参数寄存器a0~a7
const int reg_a1 = 5;       
const int reg_a2 = 6;
const int reg_a3 = 7;
const int reg_a4 = 8;
const int reg_a5 = 9;
const int reg_a6 = 10;
const int reg_a7 = 11;       //参数寄存器a7
const int reg_v0 = 4;        //函数返回值
const int reg_v1 = 5;
const int reg_t0 = 12;      //临时寄存器
const int reg_t1 = 13;
const int reg_t2 = 14;
const int reg_t3 = 15;
const int reg_t4 = 16;
const int reg_t5 = 17;
const int reg_t6 = 18;
const int reg_t7 = 19;
const int reg_t8 = 20;
const int reg_x = 21;       //保留指针未使用
const int reg_fp = 22;       //栈底指针
const int reg_s0 = 23;       //子程序寄存器变量
const int reg_s1 = 24; 
const int reg_s2 = 25; 
const int reg_s3 = 26; 
const int reg_s4 = 27; 
const int reg_s5 = 28; 
const int reg_s6 = 29; 
const int reg_s7 = 30; 
const int reg_s8 = 31; 


const int reg_fa0 = 0;      //参数寄存器
const int reg_fa1 = 1;
const int reg_fa2 = 2;
const int reg_fa3 = 3;
const int reg_fa4 = 4;
const int reg_fa5 = 5;
const int reg_fa6 = 6;
const int reg_fa7 = 7;
const int reg_fv0 = 0;      //函数返回值
const int reg_fv1 = 1;
const int reg_ft0 = 8;      //临时寄存器
const int reg_ft1 = 9; 
const int reg_ft2 = 10; 
const int reg_ft3 = 11; 
const int reg_ft4 = 12; 
const int reg_ft5 = 13; 
const int reg_ft6 = 14; 
const int reg_ft7 = 15; 
const int reg_ft8 = 16; 
const int reg_ft9 = 17; 
const int reg_ft10 = 18; 
const int reg_ft11 = 19; 
const int reg_ft12 = 20; 
const int reg_ft13 = 21; 
const int reg_ft14 = 22; 
const int reg_ft15 = 23;
const int reg_fs0 = 24;         //子程序寄存器变量
const int reg_fs1 = 25;
const int reg_fs2 = 26;
const int reg_fs3 = 27;
const int reg_fs4 = 28;
const int reg_fs5 = 29;
const int reg_fs6 = 30;
const int reg_fs7 = 31;


const std::string Ireg2name[32] = {
    "$zero", "$ra", "$tp", "$sp",
    "$a0", "$a1", "$a2", "$a3", "$a4", "$a5", "$a6", "$a7",
    "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7","$t8",
    "$x",
    "$fp",
    "$s0", "$s1","$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$s8"  
};

const std::string Freg2name[32] = {
    "$fa0", "$fa1", "$fa2", "$fa3", "$fa4", "$fa5", "$fa6", "$fa7",
    "$ft0", "$ft1", "$ft2", "$ft3", "$ft4", "$ft5", "$ft6", "$ft7","$ft8", "$ft9", "$ft10", "$ft11", "$ft12", "$ft13", "$ft14", "$ft15", 
    "$fs0", "$fs1", "$fs2", "$fs3","$fs4", "$fs5", "$fs6", "$fs7" 
};

class HLVal {
public:
    virtual bool is_reg() = 0;
    virtual bool is_const() = 0;
    virtual std::string print() = 0;
    virtual std::string get_loongarch_code() = 0;
};

class Reg: public HLVal {
public:
    Reg(int id, bool is_fp): id_(id), is_fp_(is_fp) {}
public:
    int get_id() { return id_; }
    bool is_reg() final { return true; }
    bool is_const() final { return false; }
    bool is_float() { return is_fp_; }
    std::string print();
    std::string get_loongarch_code();

private:
    int id_;
    bool is_fp_;
};

class Mem: public HLVal {
public:
    Mem(int id, int offset): id_(id), offset_(offset) {}

public:
    int get_offset() { return offset_; }
    int get_reg_id() { return id_; }
    bool is_reg() final { return false; }
    bool is_const() final { return false; }
    std::string print();
    std::string get_loongarch_code();
private:
    int id_;
    int offset_;
};

class Const: public HLVal {
public:
    Const(int ival): ival_(ival), is_fp_(false) {}
    Const(float fval): fval_(fval), is_fp_(true) {}
    bool is_const() final { return true; }
    bool is_reg() final { return false; }
    int &get_ival() { return ival_; }
    float &get_fval() { return fval_; }
    std::string print();
    std::string get_loongarch_code();
private:
    int ival_;
    float fval_;
    bool is_fp_;
};

