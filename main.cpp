//extern "C"
#include "llvm/DerivedTypes.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Support/IRBuilder.h"
#include <cstdio>
#include <string>
#include <map>
#include <vector>
#include "common.h"
using namespace llvm;
#if 0
#ifdef _cplusplus
extern "C" {
#endif
#include "2.tab.h"
#ifdef _cplusplus
#endif

#endif
#define C_EXPORT extern  "C"
////////////////////////////////////////////////////////////////////////////////////
	LLVMContext *llvm_context;
	extern "C" int yyparse(void);
	static Module *TheModule;
	static IRBuilder<> Builder(getGlobalContext());
	static std::map<std::string, Value*> NamedValues;//���������ֿռ�
    
	
	Fvalue * A_error(const char *str){fprintf(stderr,"Error: %s\n",str);return NULL;}
    
////////////////////////////////////////////////////////////////////////////
    Fvalue *AST_integer::code(){
    		printf("ast_integer::code(%d)\n",var_value);
		return ConstantInt::get(*llvm_context,APInt(32,var_value));
        
        
	}
	
	Fvalue *AST_var::code(){

		Fvalue *V = NamedValues[name];
		return V?V:NULL;

	}
	
	Fvalue *AST_bin::code(){
		Fvalue *lhs = LHS->code();
		Fvalue *rhs = RHS->code();
		printf("AST_bin:code(op:%c,)\n",op);

		if(lhs == NULL||rhs == NULL)
			return NULL;
		switch (op){
		case '+':return Builder.CreateAdd(lhs,rhs,"add");
					break;
		case '-':return Builder.CreateSub(lhs,rhs,"sub");break;
		case '*':return Builder.CreateMul(lhs,rhs,"mul");break;
		case '<':
			return Builder.CreateICmpULT(lhs,rhs,"cmp");
			//return Builder.CreateUITOInt(lhs,
			//Type::getInt32Type(*llvm_context), "bool");
			break;
		default:
			return A_error("��Ч�Ĳ�����");


		}
    }
    
    Fvalue *AST_call::code(){//����һ����������
		Function *func = TheModule->getFunction(name);
		if(func == NULL)
			return A_error("δ֪�ĺ���\n");
		if(func->arg_size() != args.size())
			return A_error("��������\n");
		std::vector<Fvalue *> Argsv; //����createcall��func�����б�

		for(unsigned int i = 0,e = args.size(); i != e; ++i){
            
			Argsv.push_back(args[i]->code());
			if ( Argsv.back() == NULL) return 0;
            
		}
		return Builder.CreateCall(func,Argsv.begin(),Argsv.end(),"call");
	}
    
    Function* AST_proto::code(){
		std::vector<const Type*> int_args(args.size(),Type::getInt32Ty(*llvm_context));	//��������
		FunctionType * func_type = FunctionType::get(Type::getInt32Ty(*llvm_context),//����ֵ����
                                                     int_args,false);
		Function	* func = Function::Create(func_type,Function::ExternalLinkage,name,TheModule);
		if(func){
            printf("proto.ok\n");
        }
            //�������
		if(func->getName() != name){
			func->eraseFromParent();
			func = TheModule->getFunction(name);
			if(!func->empty()){
				A_error("�ظ�����");
				return NULL;
			}
			if(func->arg_size()!=args.size()){
				A_error("�ظ����壬������ͬ");
			}
		}
        
        
            //���������
		int i =0;
		for(Function::arg_iterator AI = func->arg_begin();i<func->arg_size();AI++,i++){
			AI->setName(args[i]);
			NamedValues[args[i]] = AI;
		}
	//	printf("proto:%x\n",func);
		return func;
    }
    
    Function* AST_func::code(Fvalue *ret_value ){
        NamedValues.clear();//������ֿռ�(���ű�)
        Function* func = proto->code();
        if(func == NULL){
            printf("func.NULL\n");
            return NULL;
        }

        BasicBlock *func_block = BasicBlock::Create(*llvm_context,"entry",func);
        Builder.SetInsertPoint(func_block);
        Fvalue *ret_expr=NULL;
        
        if(ret_value){
        
            ret_expr = ret_value;
            }
        else{
            ret_expr=body->code();
            }
        if(ret_expr ){
            Builder.CreateRet(ret_expr);
            verifyFunction(*func);
                //func_block->end();
            return func;
        }
            //����ʧ��
        func->eraseFromParent();
        return NULL;
    }

	C_EXPORT {
        
		Function* A_FUNC_proto(FunctionType* ft, char* name, Module*module){
                        
			Function *F = Function::Create(ft, Function::ExternalLinkage, name, module);
            
			return F;
            
		}
        
		void A_FUNC_define_begin(Function *){
            
            
            
            
		}

        
        
		void A_EXP_(int a,int b,char*op){
            
			Value *va = ConstantFP::get(getGlobalContext(),APFloat((float)a));
			printf("A:%x B:%d,%s\n",a,b,op);
			Builder.CreateFAdd(va,va,"addtmp");

            
		}
        
        
	}

	int main(){
        
		LLVMContext &Context = getGlobalContext();
		llvm_context = &Context;
		TheModule = new Module("cool jit",*llvm_context);
        
		std::vector<const Type*> Doubles(2, Type::getDoubleTy(getGlobalContext()));

		FunctionType *FT = FunctionType::get(Type::getDoubleTy(getGlobalContext()),
                                             Doubles, false);
        
            //Function *F = Function::Create(FT, Function::ExternalLinkage, "main", TheModule);
		Function *F = A_FUNC_proto(FT, "main", TheModule);

        
		BasicBlock *BB = BasicBlock::Create(getGlobalContext(), "entry", F);
		Builder.SetInsertPoint(BB);
        
        
		Function::arg_iterator AI = F->arg_begin();
		AI->setName("he");
		NamedValues["he"] = AI;
        
		Value *va = ConstantFP::get(getGlobalContext(),APFloat((float)3));
		Value *vb = NULL;
		vb = Builder.CreateFAdd(NamedValues["he"],NamedValues["he"],"addtmp");
		Builder.CreateRet(vb);
/*		
		std::vector<Fstring> my_args;
		my_args.push_back("first");
		my_args.push_back("second");
		AST_integer *vint = new AST_integer(456);
		AST_var *my_var   = new AST_var("first");
		AST_proto* my_proto = new AST_proto("my_func",my_args);



		AST_func * my_func  = new AST_func (my_proto,my_var);
		std::vector<AST_expr*> my_call_args;
		my_call_args.push_back(new AST_integer(456));
		my_call_args.push_back(new AST_integer(789));
		AST_call * my_call = new AST_call("my_func",my_call_args);
		my_func->code();
  */
  		yyparse();
		//my_call->code();
		
        
		TheModule->dump();
		return 0;
        
        
	}
    
