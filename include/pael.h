#ifndef _PAEL_H_
#define _PAEL_H_

//参数表
typedef void* PAEL_ALG_PARAM_TABLE;

//变量类型
#define PAEL_VARIABLE_TYPE_NULL					0
#define PAEL_VARIABLE_TYPE_CHAR					1
#define PAEL_VARIABLE_TYPE_STRING				2
#define PAEL_VARIABLE_TYPE_USTRING				3
#define PAEL_VARIABLE_TYPE_BYTEARRAY			4
#define PAEL_VARIABLE_TYPE_DOUBLE				5
#define PAEL_VARIABLE_TYPE_FLOAT				6
#define PAEL_VARIABLE_TYPE_INTEGER				7
#define PAEL_VARIABLE_TYPE_LONG					8
#define PAEL_VARIABLE_TYPE_LIST					9


//变量
typedef struct _PAEL_VARIABLE
{
	int type;
	int size;
	
	union
	{
		char chval;
		char* szval;
		wchar_t *strval;
		unsigned char *bval;
		double dval;
		float fval;
		int nval;
		long lval;
		void* vval;
	}v;
	
}PAEL_VARIABLE,*PPAEL_VARIABLE;

//主元素
#define PAEL_DATA_ELEMENT_NAME		"_data"

//pael上下文对象
class IPaelContext
{
public:

	//获取当前完整路径
	virtual char* getPath() = 0;

	//分配内存
	virtual void* malloc(int size) = 0;
	
	//释放内存
	virtual void free(void* p) = 0;
	
	//从参数表中获取一个值
	virtual PAEL_VARIABLE getTableVal(PAEL_ALG_PARAM_TABLE table,char *name) = 0;
	
	//修改参数表中的内容
	virtual void setTableVal(PAEL_ALG_PARAM_TABLE table,char *name,PAEL_VARIABLE val) = 0;
	
	//删除一个表中的值
	virtual void deleteVal(PAEL_ALG_PARAM_TABLE table,char *name) = 0;
	
	virtual PAEL_VARIABLE createList() = 0;
	
	virtual void deleteList(PAEL_VARIABLE vallist) = 0;
	
	virtual int getListSize(PAEL_VARIABLE vallist) = 0;
	
	virtual PAEL_VARIABLE getListVal(PAEL_VARIABLE vallist,int index) = 0;
	
	virtual void appendListVal(PAEL_VARIABLE vallist,PAEL_VARIABLE val) = 0;
	
};

//算法库实现接口

class IPaelAlgorithm
{
public:
	
	//卸载库
	virtual void destroy() = 0;
	
	//执行处理算法
	virtual char* execute(IPaelContext *ctx,PAEL_ALG_PARAM_TABLE target,PAEL_ALG_PARAM_TABLE output) = 0;
	
	//执行比较算法
	virtual char* execute(IPaelContext *ctx,PAEL_ALG_PARAM_TABLE target,PAEL_ALG_PARAM_TABLE query,PAEL_ALG_PARAM_TABLE output) = 0;
};

//算法库导出接口
#ifdef __cplusplus
extern "C"{
#endif

//实现接口全部定义：char* pael_initialize(IPaelContext *ctx);
//参数：ctx pael上下文
//返回值：NULL为成功，其他为错误信息字符串
//功能：算法库初始化

//实现接口全部定义：char* pael_getAlgorithmInstance(IPaelContext *ctx,char *name,PAEL_ALG_PARAM_TABLE initparam,IPaelAlgorithm **inst);
//参数：ctx pael上下文
//		name 算法名称
//		initparam 初始化参数表
//		inst 返回算法实例对象
//返回值：NULL为成功，其他为错误信息字符串
//功能：获取算法实例对象

//实现接口全部定义：char* pael_destroy();
//参数：
//返回值：NULL为成功，其他为错误信息字符串
//算法库卸载

typedef char* (*fn_pael_initialize)(IPaelContext *ctx);

typedef char* (*fn_pael_getAlgorithmInstance)(IPaelContext *ctx,char *name,PAEL_ALG_PARAM_TABLE initparam,IPaelAlgorithm **inst);

typedef char* (*fn_pael_destroy)();

#ifdef __cplusplus
}
#endif

#endif