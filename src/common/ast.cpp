#include "ast.hpp"
#include "logging.hpp"
#include <cstring>
#include <iostream>
#include <stack>

#define _AST_NODE_ERROR_                                                       \
    std::cerr << "Abort due to node cast error."                               \
                 "Contact with TAs to solve your problem."                     \
              << std::endl;                                                    \
    std::abort();
#define _STR_EQ(a, b) (strcmp((a), (b)) == 0)

void AST::run_visitor(ASTVisitor &visitor) { root->accept(visitor); }

AST::AST(syntax_tree *s) {
    if (s == nullptr) {
        std::cerr << "empty input tree!" << std::endl;
        std::abort();
    }
    auto node = transform_node_iter(s->root);
    del_syntax_tree(s);
    root = std::shared_ptr<ASTCompUnit>(static_cast<ASTCompUnit *>(node));
}

ASTNode* AST::transform_node_iter(syntax_tree_node *n) {
    if (_STR_EQ(n->name, "CompUnit")) {
        auto node = new ASTCompUnit();
        // flatten CompUnit
        std::stack<syntax_tree_node *> cfd;
        auto list_p = n;
        while(list_p->children_num > 1){
            cfd.push(list_p->children[1]);
            list_p=list_p->children[0];
        }
        cfd.push(list_p->children[0]);

        while(!cfd.empty()){
            auto head = cfd.top();
            if(_STR_EQ(head->name, "FuncDef"))
            {
                auto child_node =
                    static_cast<ASTDeclaration *>(transform_node_iter(head));
                node->Declaration_list.push_back(std::shared_ptr<ASTDeclaration>(child_node));
            }
            else if(_STR_EQ(head->name, "Decl")){
                auto child_node =
                    static_cast<ASTDeclaration  *>(transform_node_iter(head));
                node->Declaration_list.push_back(std::shared_ptr<ASTDeclaration >(child_node));
            }
            else {
                std::cerr << "[ast]: CompUnit transform failure!"
                          << std::endl;
                std::abort();
            }
            cfd.pop();
        }
        return node;

    } else if (_STR_EQ(n->name, "Decl")) {
        return transform_node_iter(n->children[0]);
    }else if(_STR_EQ(n->name, "ConstDecl")){
        
        auto node = new ASTConstDeclaration();
        std::stack<syntax_tree_node *> constdecllist;
        if(_STR_EQ(n->children[1]->children[0]->name, "int"))
                node->type = TYPE_INT;
            else if(_STR_EQ(n->children[1]->children[0]->name, "float"))
                node->type = TYPE_FLOAT;
            else{
                std::cerr << "[ast]: CompUnit transform failure!"<<std::endl<<n->children[1]->name
                          << std::endl;
                std::abort();
            }

        if(n->children_num==5){
            auto list_p = n->children[3];
            while(list_p->children_num==3){
                constdecllist.push(list_p->children[2]);
                list_p=list_p->children[0];
            }
            constdecllist.push(list_p->children[1]);
        }

        constdecllist.push(n->children[2]);
        while(!constdecllist.empty()){
                auto head = constdecllist.top();
                auto child_node =
                    static_cast<ASTConstDef *>(transform_node_iter(head));
                node->ConstDef_list.push_back(std::shared_ptr<ASTConstDef>(child_node));
                constdecllist.pop();
        }
        return node;
        
    }else if(_STR_EQ(n->name, "VarDecl")){
            auto node = new ASTVarDeclaration();
            std::stack<syntax_tree_node *> vardecllist;
            auto list_p = n;
            if(_STR_EQ(list_p->children[0]->name, "int"))
                node->type = TYPE_INT;
            else if(_STR_EQ(list_p->children[0]->name, "float"))
                node->type = TYPE_FLOAT;
            else{
                std::cerr << "[ast]: CompUnit transform failure!"<<std::endl<<list_p->children[0]->name
                          << std::endl;
                std::abort();
            }
            list_p=list_p->children[1];
            // flatten VarDecl
            while(list_p->children_num>1){
                vardecllist.push(list_p->children[2]);
                list_p=list_p->children[0];
            }
            vardecllist.push(list_p->children[0]);

            while(!vardecllist.empty()){
                auto head = vardecllist.top();
                auto child_node =
                    static_cast<ASTVarDef *>(transform_node_iter(head));
                node->VarDef_list.push_back(std::shared_ptr<ASTVarDef>(child_node));
                vardecllist.pop();
            }
            return node;
            
    } else if (_STR_EQ(n->name, "ConstDef")) {
        auto node = new ASTConstDef();

        node->id = n->children[0]->name;
        auto constinitval=n->children[2];
        if(n->children_num>=4){
            std::stack<syntax_tree_node *> constdeflist;
            auto list_p = n->children[1];

            constinitval=n->children[3];

            while(list_p->children_num>=4){
                constdeflist.push(list_p->children[2]);
                list_p=list_p->children[0];
            }
            constdeflist.push(list_p->children[1]);

            while(!constdeflist.empty()){
                auto head = constdeflist.top();
                auto child_node =
                    static_cast<ASTConstExp *>(transform_node_iter(head));
                node->ConstExp_list.push_back(std::shared_ptr<ASTConstExp>(child_node));
                constdeflist.pop();
            }
        }
            node->ConstInitVal=std::shared_ptr<ASTConstInitVal>(static_cast<ASTConstInitVal *>(transform_node_iter(constinitval)));
            return node;

    }else if(_STR_EQ(n->name, "ConstInitVal")){
        auto node = new ASTConstInitVal();
        auto list_p = n;
        std::stack<syntax_tree_node *> constinitvallist;
        if(n->children_num==1){
            node->ConstExp = std::shared_ptr<ASTConstExp>(static_cast<ASTConstExp *>(transform_node_iter(n->children[0])));
            return node;
        }
        if(n->children_num==2){
            return node;
        }
        if(n->children_num==3){
            list_p=n->children[1];
            while(list_p->children_num==3){
                constinitvallist.push(list_p->children[2]);
                list_p=list_p->children[0];
            }
            constinitvallist.push(list_p->children[0]);

            while(!constinitvallist.empty()){
                auto head = constinitvallist.top();
                 auto child_node =
                    static_cast<ASTConstInitVal *>(transform_node_iter(head));
                node->ConstInitVal_list.push_back(std::shared_ptr<ASTConstInitVal>(child_node));
                constinitvallist.pop();
            }
            return node;
        }

    } else if (_STR_EQ(n->name, "VarDef")) {
        auto node = new ASTVarDef();
        node->id=n->children[0]->name;
        if(n->children_num == 1){
            return node;
        }
        if(_STR_EQ(n->children[1]->name, "ArrayDef")){
            auto list_p = n->children[1];
            std::stack<syntax_tree_node *> vardeflist;

            while(list_p->children_num==4){
                vardeflist.push(list_p->children[2]);
                list_p=list_p->children[0];
            }
            vardeflist.push(list_p->children[1]);

            while(!vardeflist.empty()){
                auto head = vardeflist.top();
                auto child_node =
                    static_cast<ASTConstExp *>(transform_node_iter(head));
                node->ConstExp_list.push_back(std::shared_ptr<ASTConstExp>(child_node));
                vardeflist.pop();
            }
        }
        if(n->children_num>2){
            auto initval = n->children[2];
            if(n->children_num==4){
                initval = n->children[3];
            }
            node->InitVal=std::shared_ptr<ASTInitVal>(static_cast<ASTInitVal *>(transform_node_iter(initval)));
        }
        return node;
    
    }else if(_STR_EQ(n->name, "InitVal")){
        auto node = new ASTInitVal();
        auto list_p = n;
        std::stack<syntax_tree_node *> initvallist;
        if(n->children_num==1){
            node->Exp = std::shared_ptr<ASTExp>(static_cast<ASTExp *>(transform_node_iter(n->children[0])));
            return node;
        }
        if(n->children_num==2){
            return node;
        }
        if(n->children_num==3){
            list_p=n->children[1];
            while(list_p->children_num==3){
                initvallist.push(list_p->children[2]);
                list_p=list_p->children[0];
            }
            initvallist.push(list_p->children[0]);

            while(!initvallist.empty()){
                auto head = initvallist.top();
                 auto child_node =
                    static_cast<ASTInitVal *>(transform_node_iter(head));
                node->InitVal_list.push_back(std::shared_ptr<ASTInitVal>(child_node));
                initvallist.pop();
            }
            return node;
        }

    } else if(_STR_EQ(n->name, "ConstExp")){
        auto node = new ASTConstExp();
        node->AddExp = std::shared_ptr<ASTAddExp>(static_cast<ASTAddExp *>(transform_node_iter(n->children[0])));
        return node;
    } else if(_STR_EQ(n->name, "AddExp")){
        auto node = new ASTAddExp();
        if(n->children_num==1){
            node->MulExp = std::shared_ptr<ASTMulExp>(static_cast<ASTMulExp *>(transform_node_iter(n->children[0])));
            return node;
        }
        node->AddExp = std::shared_ptr<ASTAddExp>(static_cast<ASTAddExp *>(transform_node_iter(n->children[0])));
        if(_STR_EQ(n->children[1]->name, "+")){
            node->op = OP_PLUS;
        }else {
            node->op = OP_MINUS;
        }
        node->MulExp = std::shared_ptr<ASTMulExp>(static_cast<ASTMulExp *>(transform_node_iter(n->children[2])));
        return node;
    } else if(_STR_EQ(n->name, "MulExp")){
        auto node = new ASTMulExp();
        if(n->children_num==1){
            node->UnaryExp = std::shared_ptr<ASTUnaryExp>(static_cast<ASTUnaryExp *>(transform_node_iter(n->children[0])));
            return node;
        }
        node->MulExp = std::shared_ptr<ASTMulExp>(static_cast<ASTMulExp *>(transform_node_iter(n->children[0])));
        if(_STR_EQ(n->children[1]->name, "*")){
            node->op = OP_MUL;
        }else if(_STR_EQ(n->children[1]->name, "/")){
            node->op = OP_DIV;
        }else {
            node->op = OP_MOD;
        }
        node->UnaryExp = std::shared_ptr<ASTUnaryExp>(static_cast<ASTUnaryExp *>(transform_node_iter(n->children[2])));
        return node;
    } else if(_STR_EQ(n->name, "UnaryExp")){
        auto node = new ASTUnaryExp();
        if(n->children_num==1){
            node->PrimaryExp = std::shared_ptr<ASTPrimaryExp>(static_cast<ASTPrimaryExp *>(transform_node_iter(n->children[0])));
            if(node->PrimaryExp){
            }
            return node;
        }
        if(n->children_num==2){
            if(_STR_EQ(n->children[0]->children[0]->name, "!")){
                node->op = OP_NOT;
            }else if(_STR_EQ(n->children[0]->children[0]->name, "-")){
                node->op = OP_NEG;
            }else {
                node->op = OP_POS;
            }
            node->UnaryExp = std::shared_ptr<ASTUnaryExp>(static_cast<ASTUnaryExp *>(transform_node_iter(n->children[1])));
            return node;
        }
        auto callee = new ASTCallee();
        node->Callee = std::shared_ptr<ASTCallee>(callee);
        node->Callee->id = n->children[0]->name;
        if(n->children_num==4){
            auto list_p=n->children[2];
            std::stack<syntax_tree_node *> funparaslist;

            while(list_p->children_num==3){
                funparaslist.push(list_p->children[2]);
                list_p=list_p->children[0];
            }
            funparaslist.push(list_p->children[0]);

            while(!funparaslist.empty()){
                auto head = funparaslist.top();
                auto child_node =
                    static_cast<ASTExp *>(transform_node_iter(head));
                node->Callee->ParamExp_list.push_back(std::shared_ptr<ASTExp>(child_node));
                funparaslist.pop();
            }
        }
        return node;
    } else if(_STR_EQ(n->name, "PrimaryExp")){
        auto node = new ASTPrimaryExp();
        if(n->children_num==3){
            node->Exp = std::shared_ptr<ASTExp>(static_cast<ASTExp *>(transform_node_iter(n->children[1])));
        }else if(_STR_EQ(n->children[0]->name, "LVal")){
            node->LVal = std::shared_ptr<ASTLVal>(static_cast<ASTLVal *>(transform_node_iter(n->children[0])));
        }else if(_STR_EQ(n->children[0]->name, "Number")){
            node->Number = std::shared_ptr<ASTNumber>(static_cast<ASTNumber *>(transform_node_iter(n->children[0])));
        }else {
            std::cerr << "[ast]:PrimaryExp transform failure!" << std::endl;
            std::abort();
        }
        return node;
    }else if(_STR_EQ(n->name, "LVal")){
        auto node = new ASTLVal();
        node->id = n->children[0]->name;
        if(n->children_num==2){
            auto list_p = n->children[1];
            std::stack<syntax_tree_node *> explist;

            while(list_p->children_num==4){
                explist.push(list_p->children[2]);
                list_p=list_p->children[0];
            }
            explist.push(list_p->children[1]);

            while(!explist.empty()){
                auto head = explist.top();
                auto child_node =
                    static_cast<ASTExp *>(transform_node_iter(head));
                node->ArrayExp_list.push_back(std::shared_ptr<ASTExp>(child_node));
                explist.pop();
            }
        }
        return node;

    }else if(_STR_EQ(n->name, "Number")){
        auto node = new ASTNumber();
        if(_STR_EQ(n->children[0]->name, "Integer")){
            node->type = TYPE_INT;
            node->i_val = std::stoi(n->children[0]->children[0]->name,0,0);
        }else if(_STR_EQ(n->children[0]->name, "Float")){
            node->type = TYPE_FLOAT;
            node->f_val = std::stof(n->children[0]->children[0]->name,0);
        }
        return node;
    }else if(_STR_EQ(n->name, "FuncDef")){
        auto node = new ASTFuncDef();
        if(_STR_EQ(n->children[0]->name, "int")){
            node->type = TYPE_INT;
        }else if(_STR_EQ(n->children[0]->name, "float")){
            node->type = TYPE_FLOAT;
        }else if(_STR_EQ(n->children[0]->name, "void")){
            node->type = TYPE_VOID;
        }else {
            std::cerr << "[ast]:funcdef transform failure!" <<n->children[0]->name<< std::endl;
            std::abort();
        }
        node->id = n->children[1]->name;
        auto block = n->children[4];
        if( n->children_num == 6 ){
            block = n->children[5];
            auto list_p = n->children[3];
            std::stack<syntax_tree_node *> FuncFParamlist;

            while(list_p->children_num == 3){
                FuncFParamlist.push(list_p->children[2]);
                list_p=list_p->children[0];
            }
            FuncFParamlist.push(list_p->children[0]);

            while(!FuncFParamlist.empty()){
                auto head = FuncFParamlist.top();
                auto child_node =
                    static_cast<ASTFuncFParam *>(transform_node_iter(head));
                node->FuncFParam_list.push_back(std::shared_ptr<ASTFuncFParam>(child_node));
                FuncFParamlist.pop();
            }
        }
        node->Block = std::shared_ptr<ASTBlock>(static_cast<ASTBlock *>(transform_node_iter(block)));
        return node;
    } else if(_STR_EQ(n->name, "Block")){
        auto node = new ASTBlock();
        if(n->children_num == 2){
            return node;
        }
        auto list_p = n->children[1];
        std::stack<syntax_tree_node *> blockitemlist;

        while(list_p->children_num==2){
            blockitemlist.push(list_p->children[1]);
            list_p=list_p->children[0];
        }
        blockitemlist.push(list_p->children[0]);

        while(!blockitemlist.empty()){
            auto head = blockitemlist.top();
             auto child_node =
                    static_cast<ASTBlockItem *>(transform_node_iter(head));
                node->BlockItem_list.push_back(std::shared_ptr<ASTBlockItem>(child_node));
                blockitemlist.pop();
        }
        return node;
    } else if(_STR_EQ(n->name, "FuncFParam")){
        auto node = new ASTFuncFParam();
        if(_STR_EQ(n->children[0]->children[0]->name, "int")){
            node->type = TYPE_INT;
        }else if(_STR_EQ(n->children[0]->children[0]->name, "float")){
            node->type = TYPE_FLOAT;
        }else node->type = TYPE_VOID;
        node->id=n->children[1]->name;

        if(n->children_num>2){
            node->is_array=true;
        }
        if(n->children_num==5){
            auto list_p = n->children[4];
            std::stack<syntax_tree_node *> funfparamlist;

            while(list_p->children_num==4){
                funfparamlist.push(list_p->children[2]);
                list_p=list_p->children[0];
            }
            funfparamlist.push(list_p->children[1]);

            while(!funfparamlist.empty()){
                auto head = funfparamlist.top();
                    auto child_node =
                        static_cast<ASTExp *>(transform_node_iter(head));
                    node->ParamArrayExp_list.push_back(std::shared_ptr<ASTExp>(child_node));
                    funfparamlist.pop();
            }
        }
        return node;
    }else if(_STR_EQ(n->name, "BlockItem")){
        auto node = new ASTBlockItem();
        if(_STR_EQ(n->children[0]->name, "Stmt")){
            node->Stmt=std::shared_ptr<ASTStmt>(static_cast<ASTStmt *>(transform_node_iter(n->children[0])));
            return node;
        }
        auto list_p = n->children[0];
        if(_STR_EQ(list_p->children[0]->name, "ConstDecl")){
            node->ConstDecl=std::shared_ptr<ASTConstDeclaration>(static_cast<ASTConstDeclaration *>(transform_node_iter(list_p->children[0])));
        }else {
            node->VarDecl=std::shared_ptr<ASTVarDeclaration>(static_cast<ASTVarDeclaration *>(transform_node_iter(list_p->children[0])));
        }
        return node;
    }else if(_STR_EQ(n->name, "Stmt")){
        if(_STR_EQ(n->children[0]->name, "LVal")){
            auto node = new ASTAssignStmt();
            node->LVal=std::shared_ptr<ASTLVal>(static_cast<ASTLVal *>(transform_node_iter(n->children[0])));
            node->Exp =std::shared_ptr<ASTExp>(static_cast<ASTExp *>(transform_node_iter(n->children[2])));
            return node;
        }else if(_STR_EQ(n->children[0]->name, "Exp")){
            auto node = new ASTExpStmt();
            node->Exp =std::shared_ptr<ASTExp>(static_cast<ASTExp *>(transform_node_iter(n->children[0])));
            return node;
        }else if(_STR_EQ(n->children[0]->name, "Block")){
            auto node = new ASTBlockStmt();
            node->Block = std::shared_ptr<ASTBlock>(static_cast<ASTBlock *>(transform_node_iter(n->children[0])));
            return node;
        }else if(_STR_EQ(n->children[0]->name, "if")){
            auto node = new ASTSelectStmt();
            node->Cond = std::shared_ptr<ASTCond>(static_cast<ASTCond *>(transform_node_iter(n->children[2])));
            node->IfStmt = std::shared_ptr<ASTStmt>(static_cast<ASTStmt *>(transform_node_iter(n->children[4])));
            if(n->children_num==7){
                node->ElseStmt= std::shared_ptr<ASTStmt>(static_cast<ASTStmt *>(transform_node_iter(n->children[6])));
            }
            return node;
        }else if(_STR_EQ(n->children[0]->name, "while")){
            auto node = new ASTIterationStmt();
            node->Cond = std::shared_ptr<ASTCond>(static_cast<ASTCond *>(transform_node_iter(n->children[2])));
            node->Stmt = std::shared_ptr<ASTStmt>(static_cast<ASTStmt *>(transform_node_iter(n->children[4])));
            return node;
        }else if(_STR_EQ(n->children[0]->name, "break")){
            auto node = new ASTBreakStmt();
            return node;
        }else if(_STR_EQ(n->children[0]->name, "continue")){
            auto node = new ASTContinueStmt();
            return node;
        }else if(_STR_EQ(n->children[0]->name, "return")){
            auto node = new ASTReturnStmt();
            if(n->children_num==3){
                node->Exp= std::shared_ptr<ASTExp>(static_cast<ASTExp *>(transform_node_iter(n->children[1])));
            }
            return node;
        }else if(_STR_EQ(n->children[0]->name, ";")){
            auto node = new ASTExpStmt();
            return node;
        }else {
            std::cerr << "[ast]: stmt transform failure!" << std::endl;
            std::abort();
        }
    } else if(_STR_EQ(n->name, "Cond")){
        auto node = new ASTCond();
        node->LOrExp=std::shared_ptr<ASTLOrExp>(static_cast<ASTLOrExp *>(transform_node_iter(n->children[0])));
        return node;
    } else if(_STR_EQ(n->name, "LOrExp")){
        auto node = new ASTLOrExp();
        
        if(n->children_num==1){
            node->LAndExp =std::shared_ptr<ASTLAndExp>(static_cast<ASTLAndExp *>(transform_node_iter(n->children[0])));
        }else {
            node->LOrExp =std::shared_ptr<ASTLOrExp>(static_cast<ASTLOrExp *>(transform_node_iter(n->children[0])));
            node->op=OP_OR;
            node->LAndExp =std::shared_ptr<ASTLAndExp>(static_cast<ASTLAndExp *>(transform_node_iter(n->children[2])));
        }
        return node;
    } else if(_STR_EQ(n->name, "LAndExp")){
        auto node = new ASTLAndExp();

        if(n->children_num==1){
            node->EqExp = std::shared_ptr<ASTEqExp>(static_cast<ASTEqExp *>(transform_node_iter(n->children[0])));
        }else {
            node->LAndExp =std::shared_ptr<ASTLAndExp>(static_cast<ASTLAndExp *>(transform_node_iter(n->children[0])));
            node->op=OP_AND;
            node->EqExp =std::shared_ptr<ASTEqExp>(static_cast<ASTEqExp *>(transform_node_iter(n->children[2])));
        }
        return node;
    } else if(_STR_EQ(n->name, "EqExp")){
        auto node = new ASTEqExp();
        if(n->children_num==1){
            node->RelExp = std::shared_ptr<ASTRelExp>(static_cast<ASTRelExp *>(transform_node_iter(n->children[0])));
        }else {
            node->EqExp =std::shared_ptr<ASTEqExp>(static_cast<ASTEqExp *>(transform_node_iter(n->children[0])));
            if(_STR_EQ(n->children[1]->name, "==")){
                node->op=OP_EQ;
            }else if(_STR_EQ(n->children[1]->name, "!=")){
                node->op=OP_NEQ;
            }else {
                std::cerr << "[ast]: eqexp transform failure!" << std::endl;
                std::abort();
            }
            node->RelExp =std::shared_ptr<ASTRelExp>(static_cast<ASTRelExp *>(transform_node_iter(n->children[2])));
        }
        return node;
    } else if(_STR_EQ(n->name, "RelExp")){
        auto node = new ASTRelExp();
        
        if(n->children_num==1){
            node->AddExp =std::shared_ptr<ASTAddExp>(static_cast<ASTAddExp *>(transform_node_iter(n->children[0])));
        }else {
            node->RelExp =std::shared_ptr<ASTRelExp>(static_cast<ASTRelExp *>(transform_node_iter(n->children[0])));
            if(_STR_EQ(n->children[1]->name, "<")){
                node->op=OP_LT;
            }else if(_STR_EQ(n->children[1]->name, ">")){
                node->op=OP_GT;
            }else if(_STR_EQ(n->children[1]->name, "<=")){
                node->op=OP_LE;
            }else if(_STR_EQ(n->children[1]->name, ">=")){
                node->op=OP_GE;
            }else{
                std::cerr << "[ast]: relexp transform failure!" <<std::endl<<n->children[1]->name<< std::endl;
                std::abort();
            }
            node->AddExp =std::shared_ptr<ASTAddExp>(static_cast<ASTAddExp *>(transform_node_iter(n->children[2])));
        }
        return node;
    }else if(_STR_EQ(n->name, "Exp")){
        auto node = new ASTExp();
        node->AddExp=std::shared_ptr<ASTAddExp>(static_cast<ASTAddExp *>(transform_node_iter(n->children[0])));
        return node;
    }
     else {
        std::cerr << "[ast]: transform failure!" << std::endl<<n->name<<std::endl;
        std::abort();
    }
}
/*ASTNode *AST::transform_node_iter(syntax_tree_node *n) {
    if (_STR_EQ(n->name, "program")) {
        auto node = new ASTProgram();

        // flatten declaration list
        std::stack<syntax_tree_node *> s;
        auto list_ptr = n->children[0];
        while (list_ptr->children_num == 2) {
            s.push(list_ptr->children[1]);
            list_ptr = list_ptr->children[0];
        }
        s.push(list_ptr->children[0]);

        while (!s.empty()) {
            auto child_node =
                static_cast<ASTDeclaration *>(transform_node_iter(s.top()));

            auto child_node_shared =
                std::shared_ptr<ASTDeclaration>(child_node);
            node->declarations.push_back(child_node_shared);
            s.pop();
        }
        return node;
    } else if (_STR_EQ(n->name, "declaration")) {
        return transform_node_iter(n->children[0]);
    } else if (_STR_EQ(n->name, "var-declaration")) {
        auto node = new ASTVarDeclaration();

        if (_STR_EQ(n->children[0]->children[0]->name, "int"))
            node->type = TYPE_INT;
        else
            node->type = TYPE_FLOAT;

        if (n->children_num == 3) {
            node->id = n->children[1]->name;
        } else if (n->children_num == 6) {
            node->id = n->children[1]->name;
            int num = std::stoi(n->children[3]->name);
            auto num_node = std::make_shared<ASTNum>();
            num_node->i_val = num;
            num_node->type = TYPE_INT;
            node->num = num_node;
        } else {
            std::cerr << "[ast]: var-declaration transform failure!"
                      << std::endl;
            std::abort();
        }
        return node;
    } else if (_STR_EQ(n->name, "fun-declaration")) {
        auto node = new ASTFunDeclaration();
        if (_STR_EQ(n->children[0]->children[0]->name, "int")) {
            node->type = TYPE_INT;
        } else if (_STR_EQ(n->children[0]->children[0]->name, "float")) {
            node->type = TYPE_FLOAT;
        } else {
            node->type = TYPE_VOID;
        }

        node->id = n->children[1]->name;

        // flatten params
        std::stack<syntax_tree_node *> s;
        auto list_ptr = n->children[3]->children[0];
        if (list_ptr->children_num != 0) {
            if (list_ptr->children_num == 3) {
                while (list_ptr->children_num == 3) {
                    s.push(list_ptr->children[2]);
                    list_ptr = list_ptr->children[0];
                }
            }
            s.push(list_ptr->children[0]);

            while (!s.empty()) {
                auto child_node =
                    static_cast<ASTParam *>(transform_node_iter(s.top()));

                auto child_node_shared = std::shared_ptr<ASTParam>(child_node);
                node->params.push_back(child_node_shared);
                s.pop();
            }
        }

        auto stmt_node =
            static_cast<ASTCompoundStmt *>(transform_node_iter(n->children[5]));
        node->compound_stmt = std::shared_ptr<ASTCompoundStmt>(stmt_node);
        return node;
    } else if (_STR_EQ(n->name, "param")) {
        auto node = new ASTParam();
        if (_STR_EQ(n->children[0]->children[0]->name, "int"))
            node->type = TYPE_INT;
        else
            node->type = TYPE_FLOAT;
        node->id = n->children[1]->name;
        if (n->children_num > 2)
            node->isarray = true;
        return node;
    } else if (_STR_EQ(n->name, "compound-stmt")) {
        auto node = new ASTCompoundStmt();
        if (n->children[1]->children_num == 2) {
            // flatten local declarations
            auto list_ptr = n->children[1];
            std::stack<syntax_tree_node *> s;
            while (list_ptr->children_num == 2) {
                s.push(list_ptr->children[1]);
                list_ptr = list_ptr->children[0];
            }

            while (!s.empty()) {
                auto decl_node = static_cast<ASTVarDeclaration *>(
                    transform_node_iter(s.top()));
                auto decl_node_ptr =
                    std::shared_ptr<ASTVarDeclaration>(decl_node);
                node->local_declarations.push_back(decl_node_ptr);
                s.pop();
            }
        }

        if (n->children[2]->children_num == 2) {
            // flatten statement-list
            auto list_ptr = n->children[2];
            std::stack<syntax_tree_node *> s;
            while (list_ptr->children_num == 2) {
                s.push(list_ptr->children[1]);
                list_ptr = list_ptr->children[0];
            }

            while (!s.empty()) {
                auto stmt_node =
                    static_cast<ASTStatement *>(transform_node_iter(s.top()));
                auto stmt_node_ptr = std::shared_ptr<ASTStatement>(stmt_node);
                node->statement_list.push_back(stmt_node_ptr);
                s.pop();
            }
        }
        return node;
    } else if (_STR_EQ(n->name, "statement")) {
        return transform_node_iter(n->children[0]);
    } else if (_STR_EQ(n->name, "expression-stmt")) {
        auto node = new ASTExpressionStmt();
        if (n->children_num == 2) {
            auto expr_node = static_cast<ASTExpression *>(
                transform_node_iter(n->children[0]));

            auto expr_node_ptr = std::shared_ptr<ASTExpression>(expr_node);
            node->expression = expr_node_ptr;
        }
        return node;
    } else if (_STR_EQ(n->name, "selection-stmt")) {
        auto node = new ASTSelectionStmt();

        auto expr_node =
            static_cast<ASTExpression *>(transform_node_iter(n->children[2]));
        auto expr_node_ptr = std::shared_ptr<ASTExpression>(expr_node);
        node->expression = expr_node_ptr;

        auto if_stmt_node =
            static_cast<ASTStatement *>(transform_node_iter(n->children[4]));
        auto if_stmt_node_ptr = std::shared_ptr<ASTStatement>(if_stmt_node);
        node->if_statement = if_stmt_node_ptr;

        // check whether this selection statement contains
        // else structure
        if (n->children_num == 7) {
            auto else_stmt_node = static_cast<ASTStatement *>(
                transform_node_iter(n->children[6]));
            auto else_stmt_node_ptr =
                std::shared_ptr<ASTStatement>(else_stmt_node);
            node->else_statement = else_stmt_node_ptr;
        }

        return node;
    } else if (_STR_EQ(n->name, "iteration-stmt")) {
        auto node = new ASTIterationStmt();

        auto expr_node =
            static_cast<ASTExpression *>(transform_node_iter(n->children[2]));
        auto expr_node_ptr = std::shared_ptr<ASTExpression>(expr_node);
        node->expression = expr_node_ptr;

        auto stmt_node =
            static_cast<ASTStatement *>(transform_node_iter(n->children[4]));
        auto stmt_node_ptr = std::shared_ptr<ASTStatement>(stmt_node);
        node->statement = stmt_node_ptr;

        return node;
    } else if (_STR_EQ(n->name, "return-stmt")) {
        auto node = new ASTReturnStmt();
        if (n->children_num == 3) {
            auto expr_node = static_cast<ASTExpression *>(
                transform_node_iter(n->children[1]));
            node->expression = std::shared_ptr<ASTExpression>(expr_node);
        }
        return node;
    } else if (_STR_EQ(n->name, "expression")) {
        // simple-expression
        if (n->children_num == 1) {
            return transform_node_iter(n->children[0]);
        }
        auto node = new ASTAssignExpression();

        auto var_node =
            static_cast<ASTVar *>(transform_node_iter(n->children[0]));
        node->var = std::shared_ptr<ASTVar>(var_node);

        auto expr_node =
            static_cast<ASTExpression *>(transform_node_iter(n->children[2]));
        node->expression = std::shared_ptr<ASTExpression>(expr_node);

        return node;
    } else if (_STR_EQ(n->name, "var")) {
        auto node = new ASTVar();
        node->id = n->children[0]->name;
        if (n->children_num == 4) {
            auto expr_node = static_cast<ASTExpression *>(
                transform_node_iter(n->children[2]));
            node->expression = std::shared_ptr<ASTExpression>(expr_node);
        }
        return node;
    } else if (_STR_EQ(n->name, "simple-expression")) {
        auto node = new ASTSimpleExpression();
        auto expr_node_1 = static_cast<ASTAdditiveExpression *>(
            transform_node_iter(n->children[0]));
        node->additive_expression_l =
            std::shared_ptr<ASTAdditiveExpression>(expr_node_1);

        if (n->children_num == 3) {
            auto op_name = n->children[1]->children[0]->name;
            if (_STR_EQ(op_name, "<="))
                node->op = OP_LE;
            else if (_STR_EQ(op_name, "<"))
                node->op = OP_LT;
            else if (_STR_EQ(op_name, ">"))
                node->op = OP_GT;
            else if (_STR_EQ(op_name, ">="))
                node->op = OP_GE;
            else if (_STR_EQ(op_name, "=="))
                node->op = OP_EQ;
            else if (_STR_EQ(op_name, "!="))
                node->op = OP_NEQ;

            auto expr_node_2 = static_cast<ASTAdditiveExpression *>(
                transform_node_iter(n->children[2]));
            node->additive_expression_r =
                std::shared_ptr<ASTAdditiveExpression>(expr_node_2);
        }
        return node;
    } else if (_STR_EQ(n->name, "additive-expression")) {
        auto node = new ASTAdditiveExpression();
        if (n->children_num == 3) {
            auto add_expr_node = static_cast<ASTAdditiveExpression *>(
                transform_node_iter(n->children[0]));
            node->additive_expression =
                std::shared_ptr<ASTAdditiveExpression>(add_expr_node);

            auto op_name = n->children[1]->children[0]->name;
            if (_STR_EQ(op_name, "+"))
                node->op = OP_PLUS;
            else if (_STR_EQ(op_name, "-"))
                node->op = OP_MINUS;

            auto term_node =
                static_cast<ASTTerm *>(transform_node_iter(n->children[2]));
            node->term = std::shared_ptr<ASTTerm>(term_node);
        } else {
            auto term_node =
                static_cast<ASTTerm *>(transform_node_iter(n->children[0]));
            node->term = std::shared_ptr<ASTTerm>(term_node);
        }
        return node;
    } else if (_STR_EQ(n->name, "term")) {
        auto node = new ASTTerm();
        if (n->children_num == 3) {
            auto term_node =
                static_cast<ASTTerm *>(transform_node_iter(n->children[0]));
            node->term = std::shared_ptr<ASTTerm>(term_node);

            auto op_name = n->children[1]->children[0]->name;
            if (_STR_EQ(op_name, "*"))
                node->op = OP_MUL;
            else if (_STR_EQ(op_name, "/"))
                node->op = OP_DIV;

            auto factor_node =
                static_cast<ASTFactor *>(transform_node_iter(n->children[2]));
            node->factor = std::shared_ptr<ASTFactor>(factor_node);
        } else {
            auto factor_node =
                static_cast<ASTFactor *>(transform_node_iter(n->children[0]));
            node->factor = std::shared_ptr<ASTFactor>(factor_node);
        }
        return node;
    } else if (_STR_EQ(n->name, "factor")) {
        int i = 0;
        if (n->children_num == 3)
            i = 1;
        auto name = n->children[i]->name;
        if (_STR_EQ(name, "expression") || _STR_EQ(name, "var") ||
            _STR_EQ(name, "call"))
            return transform_node_iter(n->children[i]);
        else {
            auto num_node = new ASTNum();
            if (_STR_EQ(name, "integer")) {
                num_node->type = TYPE_INT;
                num_node->i_val = std::stoi(n->children[i]->children[0]->name);
            } else if (_STR_EQ(name, "float")) {
                num_node->type = TYPE_FLOAT;
                num_node->f_val = std::stof(n->children[i]->children[0]->name);
            } else {
                _AST_NODE_ERROR_
            }
            return num_node;
        }
    } else if (_STR_EQ(n->name, "call")) {
        auto node = new ASTCall();
        node->id = n->children[0]->name;
        // flatten args
        if (_STR_EQ(n->children[2]->children[0]->name, "arg-list")) {
            auto list_ptr = n->children[2]->children[0];
            auto s = std::stack<syntax_tree_node *>();
            while (list_ptr->children_num == 3) {
                s.push(list_ptr->children[2]);
                list_ptr = list_ptr->children[0];
            }
            s.push(list_ptr->children[0]);

            while (!s.empty()) {
                auto expr_node =
                    static_cast<ASTExpression *>(transform_node_iter(s.top()));
                auto expr_node_ptr = std::shared_ptr<ASTExpression>(expr_node);
                node->args.push_back(expr_node_ptr);
                s.pop();
            }
        }
        return node;
    } else {
        std::cerr << "[ast]: transform failure!" << std::endl;
        std::abort();
    }
}*/

void ASTCompUnit::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTConstDeclaration::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTConstDef::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTConstInitVal::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTVarDeclaration::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTVarDef::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTInitVal::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTFuncDef::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTFuncFParam::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTBlock::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTAssignStmt::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTExpStmt::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTBlockStmt::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTSelectStmt::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTIterationStmt::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTBreakStmt::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTContinueStmt::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTReturnStmt::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTLVal::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTNumber::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTUnaryExp::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTCallee::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTMulExp::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTAddExp::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTRelExp::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTEqExp::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTLAndExp::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTLOrExp::accept(ASTVisitor &visitor) { visitor.visit(*this); }

void ASTDeclaration::accept(ASTVisitor &visitor)
{
    auto var_decl = dynamic_cast<ASTVarDeclaration *>(this);
    if (var_decl)
    {
        var_decl->accept(visitor);
        return;
    }
    auto const_decl = dynamic_cast<ASTConstDeclaration *>(this);
    if (const_decl)
    {
        const_decl->accept(visitor);
        return;
    }
    auto fun_decl = dynamic_cast<ASTFuncDef *>(this);
    if (fun_decl)
    {
        fun_decl->accept(visitor);
        return;
    }
    LOG(ERROR) << "Abort due to node cast error.";
}

void ASTStmt::accept(ASTVisitor &visitor)
{
    auto assign_stmt = dynamic_cast<ASTAssignStmt *>(this);
    if (assign_stmt)
    {
        assign_stmt->accept(visitor);
        return;
    }
    auto exp_stmt = dynamic_cast<ASTExpStmt *>(this);
    if (exp_stmt)
    {
        exp_stmt->accept(visitor);
        return;
    }
    auto block_stmt = dynamic_cast<ASTBlockStmt *>(this);
    if (block_stmt)
    {
        block_stmt->accept(visitor);
        return;
    }
    auto selec_stmt = dynamic_cast<ASTSelectStmt *>(this);
    if (selec_stmt)
    {
        selec_stmt->accept(visitor);
        return;
    }
    auto iter_stmt = dynamic_cast<ASTIterationStmt *>(this);
    if (iter_stmt)
    {
        iter_stmt->accept(visitor);
        return;
    }
    auto break_stmt = dynamic_cast<ASTBreakStmt *>(this);
    if (break_stmt)
    {
        break_stmt->accept(visitor);
        return;
    }
    auto continue_stmt = dynamic_cast<ASTContinueStmt *>(this);
    if (continue_stmt)
    {
        continue_stmt->accept(visitor);
        return;
    }
    auto ret_stmt = dynamic_cast<ASTReturnStmt *>(this);
    if (ret_stmt)
    {
        ret_stmt->accept(visitor);
        return;
    }
    LOG(ERROR) << "Abort due to node cast error.";
}

void ASTExp::accept(ASTVisitor &visitor)
{
    if (this->AddExp == nullptr)
        std::cout << "error null" << std::endl;
    this->AddExp->accept(visitor);
}

void ASTCond::accept(ASTVisitor &visitor)
{
    this->LOrExp->accept(visitor);
}

void ASTBlockItem::accept(ASTVisitor &visitor)
{
    if (this->ConstDecl)
    {
        this->ConstDecl->accept(visitor);
        return;
    }
    else if (this->VarDecl)
    {
        this->VarDecl->accept(visitor);
        return;
    }
    else if (this->Stmt)
    {
        this->Stmt->accept(visitor);
        return;
    }
    LOG(ERROR) << "Abort due to node cast error.";
}

void ASTPrimaryExp::accept(ASTVisitor &visitor)
{
    if (this->Exp)
    {
        this->Exp->accept(visitor);
        return;
    }
    else if (this->LVal)
    {
        this->LVal->accept(visitor);
        return;
    }
    else if (this->Number)
    {
        this->Number->accept(visitor);
        return;
    }
    LOG(ERROR) << "Abort due to node cast error.";
}

void ASTConstExp::accept(ASTVisitor &visitor)
{
    this->AddExp->accept(visitor);
}

#define _DEBUG_PRINT_N_(N)                \
    {                                     \
        std::cout << std::string(N, '-'); \
    }

#define _TYPE_(t) (((t) == ASTType::TYPE_INT) ? "int" : (((t) == ASTType::TYPE_FLOAT) ? "float" : "void"))

void ASTPrinter::visit(ASTCompUnit &node)
{
    _DEBUG_PRINT_N_(depth);
    std::cout << "compunit" << std::endl;
    add_depth();
    for (auto decl : node.Declaration_list)
    {
        decl->accept(*this);
    }
    remove_depth();
}

void ASTPrinter::visit(ASTConstDeclaration &node)
{
    _DEBUG_PRINT_N_(depth);
    std::cout << "ConstDecl" << _TYPE_(node.type) << std::endl;
    add_depth();
    for (auto constdef : node.ConstDef_list)
    {
        constdef->accept(*this);
    }
    remove_depth();
}

void ASTPrinter::visit(ASTConstDef &node)
{
    _DEBUG_PRINT_N_(depth);
    std::cout << "ConstDef:" << '\t' << node.id << std::endl;
    add_depth();
    for (auto constexp : node.ConstExp_list)
    {
        constexp->accept(*this);
    }
    if (node.ConstInitVal)
    {
        node.ConstInitVal->accept(*this);
    }
    remove_depth();
}

void ASTPrinter::visit(ASTConstInitVal &node)
{
    _DEBUG_PRINT_N_(depth);
    std::cout << "ConstInitVal" << std::endl;
    add_depth();
    if (node.ConstExp)
    {
        node.ConstExp->accept(*this);
    }
    else
    {
        for (auto constval : node.ConstInitVal_list)
        {
            constval->accept(*this);
        }
    }
    remove_depth();
}

void ASTPrinter::visit(ASTVarDeclaration &node)
{
    _DEBUG_PRINT_N_(depth);
    std::cout << "VarDecl:\t" << _TYPE_(node.type) << std::endl;
    add_depth();
    for (auto vardef : node.VarDef_list)
    {
        vardef->accept(*this);
    }
    remove_depth();
}

void ASTPrinter::visit(ASTVarDef &node)
{
    _DEBUG_PRINT_N_(depth);
    std::cout << "VarDef:" << '\t' << node.id << std::endl;
    add_depth();
    for (auto constexp : node.ConstExp_list)
    {
        constexp->accept(*this);
    }
    if (node.InitVal)
    {
        node.InitVal->accept(*this);
    }
    remove_depth();
}

void ASTPrinter::visit(ASTInitVal &node)
{
    _DEBUG_PRINT_N_(depth);
    std::cout << "InitVal" << std::endl;
    add_depth();
    if (node.Exp)
    {
        node.Exp->accept(*this);
    }
    else
    {
        for (auto constval : node.InitVal_list)
        {
            constval->accept(*this);
        }
    }
    remove_depth();
}

void ASTPrinter::visit(ASTFuncDef &node)
{
    _DEBUG_PRINT_N_(depth);
    std::cout << "FuncDef:" << '\t' << _TYPE_(node.type) << node.id << std::endl;
    add_depth();
    for (auto funcparam : node.FuncFParam_list)
    {
        funcparam->accept(*this);
    }
    if (node.Block)
    {
        node.Block->accept(*this);
    }
    remove_depth();
}

void ASTPrinter::visit(ASTFuncFParam &node)
{
    _DEBUG_PRINT_N_(depth);
    std::cout << "ParamDef:" << _TYPE_(node.type) << node.id << "\tisarray:" << node.is_array << '\n';
    add_depth();
    for (auto exp : node.ParamArrayExp_list)
    {
        if (exp != nullptr)
        {
            exp->accept(*this);
        }
    }
    remove_depth();
}

void ASTPrinter::visit(ASTBlock &node)
{
    _DEBUG_PRINT_N_(depth);
    std::cout << "Block" << node.BlockItem_list.size() << std::endl;
    add_depth();
    for (auto block_item : node.BlockItem_list)
    {
        if (block_item == nullptr)
            std::cout << "nullptr!" << std::endl;
        block_item->accept(*this);
    }
    remove_depth();
}

void ASTPrinter::visit(ASTAssignStmt &node)
{
    _DEBUG_PRINT_N_(depth);
    std::cout << "AssignStmt" << std::endl;
    add_depth();
    node.LVal->accept(*this);
    node.Exp->accept(*this);
    remove_depth();
}

void ASTPrinter::visit(ASTExpStmt &node)
{
    _DEBUG_PRINT_N_(depth);
    std::cout << "ExpStmt" << std::endl;
    if (node.Exp != nullptr)
    {
        add_depth();
        node.Exp->accept(*this);
        remove_depth();
    }
}

void ASTPrinter::visit(ASTBlockStmt &node)
{
    _DEBUG_PRINT_N_(depth);
    std::cout << "BlockStmt" << std::endl;
    add_depth();
    node.Block->accept(*this);
    remove_depth();
}

void ASTPrinter::visit(ASTSelectStmt &node)
{
    _DEBUG_PRINT_N_(depth);
    std::cout << "SelectStmt" << std::endl;
    add_depth();
    node.Cond->accept(*this);
    node.IfStmt->accept(*this);
    if (node.ElseStmt)
    {
        node.ElseStmt->accept(*this);
    }
    remove_depth();
}

void ASTPrinter::visit(ASTIterationStmt &node)
{
    _DEBUG_PRINT_N_(depth);
    std::cout << "IterationStmt" << std::endl;
    add_depth();
    node.Cond->accept(*this);
    node.Stmt->accept(*this);
    remove_depth();
}

void ASTPrinter::visit(ASTBreakStmt &node)
{
    _DEBUG_PRINT_N_(depth);
    std::cout << "BreakStmt;" << std::endl;
}

void ASTPrinter::visit(ASTContinueStmt &node)
{
    _DEBUG_PRINT_N_(depth);
    std::cout << "ContinueStmt;" << std::endl;
}

void ASTPrinter::visit(ASTReturnStmt &node)
{
    _DEBUG_PRINT_N_(depth);
    std::cout << "ReturnStmt" << std::endl;
    add_depth();
    if (node.Exp)
    {
        node.Exp->accept(*this);
    }
    remove_depth();
}

void ASTPrinter::visit(ASTLVal &node)
{
    _DEBUG_PRINT_N_(depth);
    std::cout << "Lval:\t" << node.id << std::endl;
    add_depth();
    for (auto p : node.ArrayExp_list)
    {
        p->accept(*this);
    }
    remove_depth();
}

void ASTPrinter::visit(ASTNumber &node)
{
    _DEBUG_PRINT_N_(depth);
    std::cout << "Number:\t";
    if (node.type == ASTType::TYPE_INT)
        std::cout << node.i_val << '\n';
    else
    {
        std::cout << node.f_val << '\n';
    }
}

void ASTPrinter::visit(ASTUnaryExp &node)
{
    _DEBUG_PRINT_N_(depth);
    std::cout << "UnaryExp" << std::endl;
    add_depth();
    if (node.PrimaryExp)
    {
        node.PrimaryExp->accept(*this);
    }
    else if (node.Callee)
    {
        node.Callee->accept(*this);
    }
    else
    {
        _DEBUG_PRINT_N_(depth);
        std::cout << node.op<<std::endl;
        node.UnaryExp->accept(*this);
    }
    remove_depth();
}

void ASTPrinter::visit(ASTCallee &node)
{
    _DEBUG_PRINT_N_(depth);
    std::cout << "Call:\t" << node.id << std::endl;
    add_depth();
    for (auto exp : node.ParamExp_list)
    {
        exp->accept(*this);
    }
    remove_depth();
}

void ASTPrinter::visit(ASTMulExp &node)
{
    _DEBUG_PRINT_N_(depth);
    std::cout << "MulExp:\t" << node.op << std::endl;
    add_depth();
    if (node.MulExp)
    {
        node.MulExp->accept(*this);
    }
    if (node.UnaryExp)
    {
        node.UnaryExp->accept(*this);
    }
    remove_depth();
}

void ASTPrinter::visit(ASTAddExp &node)
{
    _DEBUG_PRINT_N_(depth);
    std::cout << "AddExp:\t" << node.op << std::endl;
    add_depth();
    if (node.AddExp)
    {
        node.AddExp->accept(*this);
    }
    if (node.MulExp)
    {
        node.MulExp->accept(*this);
    }
    remove_depth();
}

void ASTPrinter::visit(ASTRelExp &node)
{
    _DEBUG_PRINT_N_(depth);
    std::cout << "RelExp:\t" << node.op << std::endl;
    add_depth();
    if (node.RelExp)
    {
        node.RelExp->accept(*this);
    }
    if (node.AddExp)
    {
        node.AddExp->accept(*this);
    }
    remove_depth();
}

void ASTPrinter::visit(ASTEqExp &node)
{
    _DEBUG_PRINT_N_(depth);
    std::cout << "EqExp:\t" << node.op << std::endl;
    add_depth();
    if (node.EqExp)
    {
        node.EqExp->accept(*this);
    }
    if (node.RelExp)
    {
        node.RelExp->accept(*this);
    }
    remove_depth();
}

void ASTPrinter::visit(ASTLAndExp &node)
{
    _DEBUG_PRINT_N_(depth);
    std::cout << "LAndExp:\t" << node.op << std::endl;
    add_depth();
    if (node.LAndExp)
    {
        node.LAndExp->accept(*this);
    }
    if (node.EqExp)
    {
        node.EqExp->accept(*this);
    }
    remove_depth();
}

void ASTPrinter::visit(ASTLOrExp &node)
{
    _DEBUG_PRINT_N_(depth);
    std::cout << "LOrExp:\t" << node.op << std::endl;
    add_depth();
    if (node.LOrExp)
    {
        node.LOrExp->accept(*this);
    }
    if (node.LAndExp)
    {
        node.LAndExp->accept(*this);
    }
    remove_depth();
}



void InitZeroJudger::visit(ASTCompUnit &node) {
    all_zero = false;
}

void InitZeroJudger::visit(ASTConstDeclaration &node) {
    all_zero = false;
}

void InitZeroJudger::visit(ASTConstDef &node) {
    all_zero = false;
}

void InitZeroJudger::visit(ASTConstInitVal &node) {
    all_zero = false;
}

void InitZeroJudger::visit(ASTVarDeclaration &node) {
    all_zero = false;
}

void InitZeroJudger::visit(ASTVarDef &node) {
    all_zero = false;
}

void InitZeroJudger::visit(ASTInitVal &node) {
    if (node.Exp) {
        node.Exp->accept(*this);
    } else {
        for (auto constval : node.InitVal_list) {
            constval->accept(*this);
        }
    }
}

void InitZeroJudger::visit(ASTFuncDef &node) {
    all_zero = false;
}

void InitZeroJudger::visit(ASTFuncFParam &node) {
    all_zero = false;
}

void InitZeroJudger::visit(ASTBlock &node) {
    all_zero = false;
}

void InitZeroJudger::visit(ASTAssignStmt &node) {
    all_zero = false;
}

void InitZeroJudger::visit(ASTExpStmt &node) {
    if (node.Exp != nullptr) {
        node.Exp->accept(*this);
    }
}

void InitZeroJudger::visit(ASTBlockStmt &node) {
    all_zero = false;
}

void InitZeroJudger::visit(ASTSelectStmt &node) {
     all_zero = false;
}

void InitZeroJudger::visit(ASTIterationStmt &node) {
    all_zero = false;
}

void InitZeroJudger::visit(ASTBreakStmt &node) {
    all_zero = false;
}

void InitZeroJudger::visit(ASTContinueStmt &node) {
    all_zero = false;
}

void InitZeroJudger::visit(ASTReturnStmt &node) {
    all_zero = false;
}

void InitZeroJudger::visit(ASTLVal &node) {
    all_zero = false;
}

void InitZeroJudger::visit(ASTNumber &node) {
    if (node.type == ASTType::TYPE_INT) {
        if(node.i_val!=0) {
            all_zero = false;
            return;
        }
    } else {
        if(node.f_val!=0){
            all_zero = false;
            return;
        }
    }
}

void InitZeroJudger::visit(ASTUnaryExp &node) {
    if (node.PrimaryExp) {
        node.PrimaryExp->accept(*this);
    } else if (node.Callee) {
        all_zero = false;
    } else {
        all_zero = false;
    }
}

void InitZeroJudger::visit(ASTCallee &node) {
    all_zero = false;
}

void InitZeroJudger::visit(ASTMulExp &node) {
    if (node.MulExp) {
            all_zero = false;
    }
    if (node.UnaryExp) {
        node.UnaryExp->accept(*this);
    }
}

void InitZeroJudger::visit(ASTAddExp &node)
{
    if (node.AddExp) {
            all_zero = false;
    }
    if (node.MulExp) {
        node.MulExp->accept(*this);
    }
}

void InitZeroJudger::visit(ASTRelExp &node) {
    all_zero = false;
}

void InitZeroJudger::visit(ASTEqExp &node) {
    all_zero = false;
}

void InitZeroJudger::visit(ASTLAndExp &node) {
    all_zero = false;
}

void InitZeroJudger::visit(ASTLOrExp &node) {
    all_zero = false;
}

