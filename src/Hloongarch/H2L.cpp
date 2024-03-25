#include "../include/Hloongarch/H2L.hpp"
#include "../include/Hloongarch/HLLoc.hpp"
#include "../include/Hloongarch/HLValue.hpp"


#define __L_TRIPLE_REG_OP__(op, rd, rs1, rs2)         \
        (H2L::space + (op) + H2L::space + (rd)->get_loongarch_code() + ", " + (rs1)->get_loongarch_code() + ", "+ (rs2)->get_loongarch_code() + H2L::newline) 

namespace H2L {

//& immediate instructions
//& 外部调用保证rs不可能为s1号寄存器
std::string addi(Reg *rd, Reg *rs, int imm) {
    std::string loongarch_code;
    Reg* tmp_s1 = new Reg(reg_s1, false);
    if(imm <= 2047 && imm >= -2048) {
        loongarch_code += H2L::space + "addi" + H2L::space + rd->get_loongarch_code() + ", " + rs->get_loongarch_code() + ", " + std::to_string(imm) + H2L::newline;
    } else {
        loongarch_code += H2L::space + "li" + H2L::space + tmp_s1->get_loongarch_code() + ", " + std::to_string(imm) + H2L::newline;
        loongarch_code += __L_TRIPLE_REG_OP__("add", rd, rs, tmp_s1);
    }
    return loongarch_code;
}

std::string addiw(Reg *rd, Reg *rs, int imm) {
    std::string loongarch_code;
    Reg* tmp_s1 = new Reg(reg_s1, false);
    if(imm <= 2047 && imm >= -2048) {
        loongarch_code += H2L::space + "addiw" + H2L::space + rd->get_loongarch_code() + ", " + rs->get_loongarch_code() + ", " + std::to_string(imm) + H2L::newline;
    } else {
        loongarch_code += H2L::li(tmp_s1, imm);
        loongarch_code += __L_TRIPLE_REG_OP__("addw", rd, rs, tmp_s1);
    }
    return loongarch_code;
}

std::string li(Reg *rd, int imm) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "li" + H2L::space + rd->get_loongarch_code() + ", " + std::to_string(imm) + H2L::newline;
    return loongarch_code;
}

std::string li(Reg *rd, uint32_t fliteral) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "li" + H2L::space + rd->get_loongarch_code() + ", " + std::to_string(fliteral) + H2L::newline;
    return loongarch_code;
}


//& 3 regs alu instructions
std::string add(Reg *rd, Reg *rs1, Reg *rs2) {
    std::string loongarch_code;
    loongarch_code += __L_TRIPLE_REG_OP__("add", rd, rs1, rs2);
    return loongarch_code;
}

std::string addw(Reg *rd, Reg *rs1, Reg *rs2) {
    std::string loongarch_code;
    loongarch_code += __L_TRIPLE_REG_OP__("addw", rd, rs1, rs2);
    return loongarch_code;
}

std::string subw(Reg *rd, Reg *rs1, Reg *rs2) {
    std::string loongarch_code;
    loongarch_code += __L_TRIPLE_REG_OP__("subw", rd, rs1, rs2);
    return loongarch_code;
}

std::string mulw(Reg *rd, Reg *rs1, Reg *rs2) {
    std::string loongarch_code;
    loongarch_code += __L_TRIPLE_REG_OP__("mulw", rd, rs1, rs2);
    return loongarch_code;
}

std::string mul(Reg *rd, Reg *rs1, Reg *rs2) {
    std::string loongarch_code;
    loongarch_code += __L_TRIPLE_REG_OP__("mul", rd, rs1, rs2);
    return loongarch_code;
}

std::string divw(Reg *rd, Reg *rs1, Reg *rs2) {
    std::string loongarch_code;
    loongarch_code += __L_TRIPLE_REG_OP__("divw", rd, rs1, rs2);
    return loongarch_code;
}


std::string remw(Reg *rd, Reg *rs1, Reg *rs2) {
    std::string loongarch_code;
    loongarch_code += __L_TRIPLE_REG_OP__("remw", rd, rs1, rs2);
    return loongarch_code;
}

std::string slliw(Reg *rd, Reg *rs, int imm) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "slliw" + H2L::space + rd->get_loongarch_code() + ", " + rs->get_loongarch_code() + ", " + std::to_string(imm) + H2L::newline;
    return loongarch_code;
}


std::string slli(Reg *rd, Reg *rs, int imm) {
    std::string code;
    code += H2L::space + "slli" + H2L::space + rd->get_loongarch_code() + ", " + rs->get_loongarch_code() + ", " + std::to_string(imm) + H2L::newline;
    return code;
}

std::string sraiw(Reg *rd, Reg *rs, int imm) {
     std::string loongarch_code;
    loongarch_code += H2L::space + "sraiw" + H2L::space + rd->get_loongarch_code() + ", " + rs->get_loongarch_code() + ", " + std::to_string(imm) + H2L::newline;
    return loongarch_code;
}

std::string srai(Reg *rd, Reg *rs, int imm) {
     std::string code;
    code += H2L::space + "srai" + H2L::space + rd->get_loongarch_code() + ", " + rs->get_loongarch_code() + ", " + std::to_string(imm) + H2L::newline;
    return code;
}

std::string srliw(Reg *rd, Reg *rs, int imm) {
     std::string loongarch_code;
    loongarch_code += H2L::space + "srliw" + H2L::space + rd->get_loongarch_code() + ", " + rs->get_loongarch_code() + ", " + std::to_string(imm) + H2L::newline;
    return loongarch_code;
}

std::string srli(Reg *rd, Reg *rs, int imm) {
     std::string code;
    code += H2L::space + "srli" + H2L::space + rd->get_loongarch_code() + ", " + rs->get_loongarch_code() + ", " + std::to_string(imm) + H2L::newline;
    return code;
}

std::string fadds(Reg *rd, Reg *rs1, Reg *rs2) {
    std::string loongarch_code;
    loongarch_code += __L_TRIPLE_REG_OP__("fadd.s", rd, rs1, rs2);
    return loongarch_code;
}

std::string fsubs(Reg *rd, Reg *rs1, Reg *rs2) {
    std::string loongarch_code;
    loongarch_code += __L_TRIPLE_REG_OP__("fsub.s", rd, rs1, rs2);
    return loongarch_code;
}

std::string fmuls(Reg *rd, Reg *rs1, Reg *rs2) {
    std::string loongarch_code;
    loongarch_code += __L_TRIPLE_REG_OP__("fmul.s", rd, rs1, rs2);
    return loongarch_code;
}

std::string fdivs(Reg *rd, Reg *rs1, Reg *rs2) {
    std::string loongarch_code;
    loongarch_code += __L_TRIPLE_REG_OP__("fdiv.s", rd, rs1, rs2);
    return loongarch_code;
}

std::string fnegs(Reg *rd) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "fneg.s	" + H2L::space + rd->get_loongarch_code() + ", " + rd->get_loongarch_code() + H2L::newline;
    return loongarch_code;
}

//& logical ops
std::string andi(Reg *rd, Reg *rs, int imm) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "andi" + H2L::space + rd->get_loongarch_code() + ", " + rs->get_loongarch_code() + ", " + std::to_string(imm)+ H2L::newline;
    return loongarch_code;
}

std::string land(Reg *rd, Reg *rs1, Reg *rs2) {
    std::string loongarch_code;
    loongarch_code += __L_TRIPLE_REG_OP__("and", rd, rs1, rs2);
    return loongarch_code;
}

//& cmp ops
std::string seqz(Reg *rd, Reg *rs) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "seqz" + H2L::space + rd->get_loongarch_code() + ", " + rs->get_loongarch_code() + H2L::newline;
    return loongarch_code;
}

std::string snez(Reg *rd, Reg *rs) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "snez" + H2L::space + rd->get_loongarch_code() + ", " + rs->get_loongarch_code() + H2L::newline;
    return loongarch_code;
}

std::string feqs(Reg *rd1, Reg *rs1, Reg *rs2) {
    std::string loongarch_code;
    loongarch_code += __L_TRIPLE_REG_OP__("feq.s", rd1, rs1, rs2);
    return loongarch_code;
}

std::string fles(Reg *rd1, Reg *rs1, Reg *rs2) {
    std::string loongarch_code;
    loongarch_code += __L_TRIPLE_REG_OP__("fle.s", rd1, rs1, rs2);
    return loongarch_code;
}

std::string flts(Reg *rd1, Reg *rs1, Reg *rs2) {
    std::string loongarch_code;
    loongarch_code += __L_TRIPLE_REG_OP__("flt.s", rd1, rs1, rs2);
    return loongarch_code;
}

//& cond branches

std::string beq(Reg *rs1, Reg *rs2, Label *label) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "beq" + H2L::space + rs1->get_loongarch_code() + ", " + rs2->get_loongarch_code() + ", " + label->get_loongarch_code() + H2L::newline;
    return loongarch_code;
}

std::string bne(Reg *rs1, Reg *rs2, Label *label) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "bne" + H2L::space + rs1->get_loongarch_code() + ", " + rs2->get_loongarch_code() + ", " + label->get_loongarch_code() + H2L::newline;
    return loongarch_code;
}

std::string bge(Reg *rs1, Reg *rs2, Label *label) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "bge" + H2L::space + rs1->get_loongarch_code() + ", " + rs2->get_loongarch_code() + ", " + label->get_loongarch_code() + H2L::newline;
    return loongarch_code;
}

std::string blt(Reg *rs1, Reg *rs2, Label *label) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "blt" + H2L::space + rs1->get_loongarch_code() + ", " + rs2->get_loongarch_code() + ", " + label->get_loongarch_code() + H2L::newline;
    return loongarch_code;
}

//& mem ops
std::string sd(Reg *src, Reg *base, int offset) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "sd" + H2L::space + src->get_loongarch_code() + ", " + std::to_string(offset) +"(" + base->get_loongarch_code() + ")" + H2L::newline;
    return loongarch_code;
}

std::string ld(Reg *dst, Reg *base, int offset) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "ld" + H2L::space + dst->get_loongarch_code() + ", " + std::to_string(offset) +"(" + base->get_loongarch_code() + ")" + H2L::newline;
    return loongarch_code;
}

std::string sw(Reg *src, Reg *base, int offset) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "sw" + H2L::space + src->get_loongarch_code() + ", " + std::to_string(offset) +"(" + base->get_loongarch_code() + ")" + H2L::newline;
    return loongarch_code;
}

std::string lw(Reg *dst, Reg *base, int offset) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "lw" + H2L::space + dst->get_loongarch_code() + ", " + std::to_string(offset) +"(" + base->get_loongarch_code() + ")" + H2L::newline;
    return loongarch_code;
}

std::string fsw(Reg *src, Reg *base, int offset) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "fsw" + H2L::space + src->get_loongarch_code() + ", " + std::to_string(offset) +"(" + base->get_loongarch_code() + ")" + H2L::newline;
    return loongarch_code;
}

std::string flw(Reg *dst, Reg *base, int offset) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "flw" + H2L::space + dst->get_loongarch_code() + ", " + std::to_string(offset) +"(" + base->get_loongarch_code() + ")" + H2L::newline;
    return loongarch_code;
}

//& mv ops
std::string mv(Reg *dst, Reg *src) {
    std::string loongarch_code;
    if(dst->get_id() == src->get_id())
        return loongarch_code;
    loongarch_code += H2L::space + "mv" + H2L::space + dst->get_loongarch_code() + ", " + src->get_loongarch_code() + H2L::newline;
    return loongarch_code;
} 

std::string fmvs(Reg *dst, Reg *src) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "fmv.s" + H2L::space + dst->get_loongarch_code() + ", " + src->get_loongarch_code() + H2L::newline;
    return loongarch_code;
}

//& cast move
std::string fmvsx(Reg *dst, Reg *src) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "fmv.s.x" + H2L::space + dst->get_loongarch_code() + ", " + src->get_loongarch_code() + H2L::newline;
    return loongarch_code;
}

//& unconditional branch
std::string j(Label *label) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "j" + H2L::space + label->get_loongarch_code() + H2L::newline;
    return loongarch_code;
}


//& cast ops
std::string fcvtsw(Reg *dst, Reg *src) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "fcvt.s.w" + H2L::space + dst->get_loongarch_code() + ", " + src->get_loongarch_code() + H2L::newline;
    return loongarch_code;
}

std::string fcvtws(Reg *dst, Reg *src) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "fcvt.w.s" + H2L::space + dst->get_loongarch_code() + ", " + src->get_loongarch_code() + ", rtz" + H2L::newline;
    return loongarch_code;
}


//& load address
std::string la(Reg *rd, Label *label) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "la" + H2L::space + rd->get_loongarch_code() + ", " + label->get_loongarch_code() + H2L::newline;
    return loongarch_code;
}

//& sext
std::string sextw(Reg *rd, Reg *rs) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "sext.w" + H2L::space + rd->get_loongarch_code() + ", " + rs->get_loongarch_code() + H2L::newline;
    return loongarch_code;
}

//& ret
std::string ret() {
    std::string loongarch_code;
    loongarch_code += H2L::space + "ret" + H2L::newline;
    return loongarch_code;
}

//& call
std::string call(Label *label) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "call" + H2L::space + label->get_loongarch_code() + H2L::newline;
    return loongarch_code;
}

}