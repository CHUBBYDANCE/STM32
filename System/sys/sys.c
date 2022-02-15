#include "sys.h"



#ifdef  USE_FULL_ASSERT
//当编译提示出错的时候此函数用来报告错误的文件和所在行
//file：指向源文件
//line：指向在文件中的行数
void assert_failed(uint8_t* file, uint32_t line)
{ 
	while (1)
	{
	}
}
#endif
//THUMB指令不支持汇编内联
//采用如下方法实现执行汇编指令WFI  
//THUMB指令不支持汇编内联
//采用如下方法实现执行汇编指令WFI  
void WFI_SET(void)
{
	asm("WFI");		  
}
//关闭所有中断(但是不包括fault和NMI中断)
void INTX_DISABLE(void)
{		
    asm("CPSID   I");	
    asm("BX      LR");	
}
//开启所有中断
void INTX_ENABLE(void)
{
    asm("CPSIE   I");	
    asm("BX      LR");	
}
//设置栈顶地址
//addr:栈顶地址
void MSR_MSP(u32 addr) 
{
    asm("MSR MSP, r0");	//set Main Stack value
    asm("BX r14");	
}
