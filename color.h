/***************************************************************
Copyright © wkangk <wangkangchn@163.com>
文件名		: color.h
作者	  	: wkangk <wangkangchn@163.com>
版本	   	: v1.0
描述	   	: 控制台颜色
时间	   	: 2020-08-13 21:27
***************************************************************/
#ifndef __COLOR_H__
#define __COLOR_H__

/* 
\033[特殊属性;背景颜色(空颜色);前景颜色(字颜色)m 待输出字符串 \033[0m

总的来说就是 \033[属性 这个东西来设置后面字符串输出的颜色!!!
\033[0m 是关闭所有的属性, 使用默认的颜色输出

字色              背景              颜色
---------------------------------------
30                40              黑色
31                41              紅色
32                42              綠色
33                43              黃色
34                44              藍色
35                45              紫紅色
36                46              青藍色
37                47              白色

\033[0m  关闭所有属性    
\033[1m   设置高亮度    
\033[3m   倾斜    
\033[4m   下划线    
\033[5m   闪烁    
\033[7m   反显    
\033[8m   消隐    
\033[30m   --   \033[37m   设置前景色    
\033[40m   --   \033[47m   设置背景色

 */

#define NONE            "\033[0m"  
#define BLACK           "\033[0;30m"  
#define LIGHT_BLACK     "\033[1;30m"  
#define RED             "\033[0;31m"  
#define LIGHT_RED       "\033[1;31m"  
#define GREEN           "\033[0;32m"  
#define LIGHT_GREEN     "\033[1;32m" 
#define YELLOW          "\033[0;33m"  
#define LIGHT_YELLOW    "\033[1;33m"  
#define BLUE            "\033[0;34m"  
#define LIGHT_BLUE      "\033[1;34m"  
#define PURPLE          "\033[0;35m"  
#define LIGHT_PURPLE    "\033[1;35m"  
#define CYAN            "\033[0;36m"  
#define LIGHT_CYAN      "\033[1;36m"  
#define WHITE           "\033[0;37m"  
#define LIGHT_WHITE     "\033[1;37m" 

#endif // !__COLOR_H__