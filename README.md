# Linux C 用户态小工具
Linux C 用户态小工具  
wkangk <<wangkangchn@163.com>>  

  
*********************************************************************  
    2020-09-03 14:04  
    -----------------------------------------------------------------  
    1. 添加时间测试宏     
*********************************************************************  

*********************************************************************   
    2020-08-31 21:56
    -----------------------------------------------------------------   
    1. 修改 stack.h 宏添加(), 预防莫名的错误    
*********************************************************************  

*********************************************************************   
    2020-08-30 16:04  
    -----------------------------------------------------------------   
    1. 修改 calloc_buf, 添加对 calloc 返回值的断言  
    2. 删除宏 CHECK_PTR_IS_EXIST  
*********************************************************************  

*********************************************************************   
    2020-08-23 18:41  
    -----------------------------------------------------------------   
    1. 添加数组实现栈与队列  
*********************************************************************  

*********************************************************************   
    2020-08-15 11:37  
    -----------------------------------------------------------------   
    1. list.h 将内核态list.h迁入用户态  
    2. log.h 用户态实现printf的分等级输出, 并可将信息输入到日志文件中, 详细使用方法见log.h  
    3. tools.h 常用工具宏    
*********************************************************************