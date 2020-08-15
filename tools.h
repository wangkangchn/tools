/***************************************************************
Copyright © wangk <wangkangchn@163.com>
文件名		: my_user_tools.h
作者	  	: wangk <wangkangchn@163.com>
版本	   	: v1.0
描述	   	: 用户空间便捷工具
时间	   	: 2020-08-04 21:37
***************************************************************/
#ifndef __MY_USER_TOOLS_H__
#define __MY_USER_TOOLS_H__

/* 标记 */
#define LAB(name)               lab_ ## name    

/* 数组大小 */
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0])) 

#endif // !__MY_USER_TOOLS_H__

