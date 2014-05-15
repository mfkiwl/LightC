#ifndef ANODE_H
#define ANODE_H
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>

#define DEFTREECODE(sym, name, type, len) sym,
enum anode_code{
	#include "c-tree.def"
};
#undef DEFTREECODE

class anode_node;
typedef anode_node* anode;

extern int anode_code_length(int node);
extern int anode_code(anode node);
extern int anode_code_class(int code);
extern const char* anode_code_name(int code);
void anode_class_check_failed(const anode node, int cl, const char *file,
                int line, const char* function);
void anode_check_failed(const anode node, enum anode_code code, const char*file,
		int line, const char* function);
void anode_operand_check_failed (int idx, enum anode_code code, const char *file,
                                int line, const char *function);
#define ANODE_CLASS_CHECK(T, CLASS) __extension__                        \
({  __typeof(T) __t = (T);                                               \
    if (anode_code_class (anode_code(__t)) != (CLASS))                    \
      anode_class_check_failed (__t, (CLASS), __FILE__, __LINE__,        \
                               __FUNCTION__);                           \
    __t; })

#define ANODE_CHECK(T, CODE) __extension__                               \
({  __typeof(T) __t = (T);                                               \
    if (anode_code(__t) != (CODE))                                      \
      anode_check_failed (__t, (CODE), __FILE__, __LINE__, __FUNCTION__); \
    __t; })

#define IS_EXPR_CODE_CLASS(CLASS) (strchr("<12ers", (CLASS)) != 0)
#define EXPR_P(Node) IS_EXPR_CODE_CLASS(anode_code_class(anode_code(Node)))

#define EXPR_CHECK(T) __extension__                                     \
({  anode __t = (T);                                              \
    if (!EXPR_P(__t))                                                   \
            anode_class_check_failed(__t, 'e', __FILE__, __LINE__,      \
                __FUNCTION__);                                          \
    __t;})

#define ANODE_OPERAND_CHECK(T, I) __extension__                         \
(*({ __typeof(T) __t = (T);                                             \
        const int __i = (I);                                            \
        if (__i < 0 || __i >= anode_code_length(anode_code(T)))         \
            anode_operand_check_failed (__i, anode_code(__t),           \
                __FILE__, __LINE__, __FUNCTION__);                      \
        &__t->operands[__i];}))

#define ANODE_OPERAND_CHECK_CODE(T, CODE, I) __extension__              \
(*({ __typeof(T) __t = (T);                                             \
     const int   __i = (I);                                             \
    if (anode_code(__t) != CODE)                                        \
        anode_check_failed(__t, CODE, __FILE__, __LINE__,__FUNCTION__); \
    if (__i < 0 || __i >= anode_code_length(CODE))                      \
        anode_operand_check_failed (__i, (CODE), __FILE__, __LINE__,    \
            __FUNCTION__);                                              \
    &__t->operands[__i];}))



#define ANODE_(Node, Type) __extension__ 				\
({  ((Type*)(Node)); })
#define ANODE_CHAIN(Node)       ANODE_CHECK((Node), ANODE_LIST)->chain
#define ANODE_LIST_CHECK
#define ANODE_VALUE(Node)       ANODE_(ANODE_CHECK((Node), ANODE_LIST),anode_list)->value
/* Every node has one type */
#define ANODE_TYPE(Node)        ((Node)->type)
#define ANODE_DECL_TYPE(Node)   (ANODE_CLASS_CHECK((Node),'d')->type)

#define IDENTIFIER_POINTER(Node) ANODE_CHECK(ANODE_(Node, anode_identifier), IDENTIFIER_NODE)->pointer

#define ANODE_OPERAND(NODE, I)  ANODE_OPERAND_CHECK(NODE, I)
#define IF_STMT_CHECK(NODE)     ANODE_CHECK(NODE, IF_STMT)
#define IF_COND(NODE)           ANODE_OPERAND (IF_STMT_CHECK (NODE), 0)
#define THEN_CLAUSE(NODE)       ANODE_OPERAND (IF_STMT_CHECK (NODE), 1)
#define ELSE_CLAUSE(NODE)       ANODE_OPERAND (IF_STMT_CHECK (NODE), 2)

class anode_node{
public:
        anode           chain;
        unsigned        code;
        anode           type; /* each has a type info */

        unsigned side_effects_flag : 1;
        unsigned constant_flag : 1;
        unsigned addressable_flag : 1;
        unsigned volatile_flag : 1;
        unsigned readonly_flag : 1;
        unsigned unsigned_flag : 1;
        unsigned asm_written_flag: 1;
        unsigned unused_0 : 1;

        unsigned used_flag : 1;
        unsigned nothrow_flag : 1;
        unsigned static_flag : 1;
        unsigned public_flag : 1;
        unsigned private_flag : 1;
        unsigned protected_flag : 1;
        unsigned deprecated_flag : 1;
        unsigned unused_1 : 1;

        unsigned lang_flag_0 : 1;
        unsigned lang_flag_1 : 1;
        unsigned lang_flag_2 : 1;
        unsigned lang_flag_3 : 1;
        unsigned lang_flag_4 : 1;
        unsigned lang_flag_5 : 1;
        unsigned lang_flag_6 : 1;
        unsigned unused_2 : 1;
public:
	virtual int length(){
		return anode_code_length(this->code);
	}
    anode_node(){
        chain = NULL;
    }
	virtual ~anode_node(){
	}
};

typedef struct location_s{
        const char *file;
        int line;
}location_t;

class anode_identifier : public anode_node{
public:
	anode_identifier(char * s){
		code = IDENTIFIER_NODE;
		pointer = strdup(s);
	}
	~anode_identifier(){
		printf("free\n");
		free(pointer);
	}
	char *pointer;
};
class anode_int_cst : public anode_node {
public:
	anode_int_cst(int cst):int_cst(cst){
		code = INTEGER_CST;
	}
	int 	int_cst;
};
class anode_string : public anode_node{
public:
	anode_string(char *str){
		code = STRING_CST;
		pointer = strdup(str);
		length = strlen(str);
	}
	~anode_string(){
		free((void*)pointer);
	}
public:
        int length;
        const char *pointer;
};
class anode_list : public anode_node{
public:
    anode_list():value(NULL),purpose(NULL){
        chain = NULL;
        code = ANODE_LIST;
    }
public:
        anode purpose;
        anode value;
};
class anode_type : public anode_node {
public:
	anode_type(int c){
		code = c;
	}

public:
        anode values;
        anode size;
        anode size_unit;
        anode attributes;
        unsigned int uid; 

        unsigned int precision : 9; 
        unsigned int mode : 7; 

        unsigned string_flag : 1; 
        unsigned no_force_blk_flag : 1; 
        unsigned needs_constructing_flag : 1; 
        unsigned transparent_union_flag : 1; 
        unsigned packed_flag : 1; 
        unsigned restrict_flag : 1; 
        unsigned spare : 2; 

        unsigned lang_flag_0 : 1; 
        unsigned lang_flag_1 : 1; 
        unsigned lang_flag_2 : 1; 
        unsigned lang_flag_3 : 1; 
        unsigned lang_flag_4 : 1; 
        unsigned lang_flag_5 : 1; 
        unsigned lang_flag_6 : 1; 
        unsigned user_align  : 1;
        unsigned int align;
        anode pointer_to;
        anode reference_to;
        union anode_type_symtab {
                int address;
                char *pointer;
               // struct die_struct *die;
        } symtab;
        anode name;
        anode minval;
        anode maxval;
        anode next_variant;
        anode main_variant;
        anode binfo;
        anode context;
        int alias_set;
};
//
class anode_decl : public anode_node{
public:
public:
        unsigned uid;
        anode name;
        anode context;

        anode           size;
        unsigned char mode : 8;

        unsigned external_flag : 1;
        unsigned nonlocal_flag : 1;
        unsigned regdecl_flag : 1;
        unsigned inline_flag : 1;
        unsigned bit_field_flag : 1;
        unsigned virtual_flag : 1;
        unsigned ignored_flag : 1;
        unsigned abstract_flag : 1;

        unsigned in_system_header_flag : 1;
        unsigned common_flag : 1;
        unsigned defer_output : 1;
        unsigned transparent_union : 1;
        unsigned static_ctor_flag : 1;
        unsigned static_dtor_flag : 1;
        unsigned artificial_flag : 1;
        unsigned weak_flag : 1;

        unsigned non_addr_const_p : 1;
        unsigned no_instrument_function_entry_exit : 1;
        unsigned comdat_flag : 1;
        unsigned malloc_flag : 1;
        unsigned no_limit_stack : 1;
        unsigned built_in_class : 2;
        unsigned pure_flag : 1;

        unsigned non_addressable : 1;
        unsigned user_align : 1;
        unsigned uninlinable : 1;
        unsigned thread_local_flag : 1;
        unsigned declared_inline_flag : 1;
        unsigned visibility : 2;
        unsigned unused : 1;



        unsigned lang_flag_0 : 1;
        unsigned lang_flag_1 : 1;
        unsigned lang_flag_2 : 1;
        unsigned lang_flag_3 : 1;
        unsigned lang_flag_4 : 1;
        unsigned lang_flag_5 : 1;
        unsigned lang_flag_6 : 1;
        unsigned lang_flag_7 : 1;

        union anode_decl_u1{
        struct anode_decl_u1_a {
                unsigned int align : 24;
                unsigned int off_align : 8;
        } a;
        }u1;

        anode size_unit;
        anode arguments;
        anode result;
        anode initial;
        anode abstract_origin;
        anode assembler_name;
        anode section_name;
        anode attributes;

        anode saved_func; /* for FUNCTION_DECL , is DECL_SAVED_TREE */

        int pointer_alias_set;

};

inline anode decl_name(anode node){
	anode_decl *t = ANODE_(node, anode_decl);
	return ANODE_CLASS_CHECK(t, 'd')->name;
}
inline anode decl_initial(anode node){
	anode_decl *t = (anode_decl*)node;
	return ANODE_CLASS_CHECK(t, 'd')->initial;
}

class anode_expr : public anode_node {
public:
        anode_expr(int c){
            int len = anode_code_length(c);
            operands = (anode*)malloc(sizeof(anode) * len);
            memset(operands, 0, sizeof(anode) * len);

            code = c;
        }
        ~anode_expr(){
            free(operands);
        }
public:
        int comp;
        anode *operands;

};
class anode_block : public anode_node {
public:
        unsigned handler_block_flag : 1;
        unsigned abstract_flag : 1;
        unsigned block_num : 30;

        anode vars;
        anode subblocks;
        anode supercontext;
        anode abstract_origin;
        anode fragment_origin;
        anode fragment_chain;
};

anode build_list(anode root, anode node);
anode chain_cat(anode a, anode b);
anode build_decl(anode specifier, anode declarator);
anode anode_cons(anode purpose, anode node, anode chain);
void c_parse_init(void);
anode build_stmt(int code, ...);
#endif