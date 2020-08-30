/***************************************************************
Copyright © wkangk <wangkangchn@163.com>
文件名		: signo.c
作者	  	: wkangk <wangkangchn@163.com>
版本	   	: v1.0
描述	   	: 自动分配信号号
时间	   	: 2020-08-13 08:47
***************************************************************/
#include "signo.h"

pool signo_pool = 0;

/**
 * get_signo - 申请自定义信号
 * @return: 正: 申请成功的信号号, -1失败
 */
int get_signo(void)
{
    int i, ret = -1;
    /* 逐位检验时候线程号是否被使用，0未被使用，1被使用 */
    for (i = 0; i < POLL_SIZE; ++i) {
        /* 验证第i位是否使用 */
        /* 当第i位为0时, >> (i - 1) 是 >> i 的两倍 (>> (i - 1)) - (>> i) == (>> i)
           当第i位为1时, (>> (i - 1)) - (>> i) != (>> i)*/
        if ((signo_pool >> i) - (signo_pool >> (i + 1)) == (signo_pool >> (i + 1))) {
            continue;
        } else {    /* == 0 */
            signo_pool = signo_pool | ((typeof(signo_pool))1 << i); /* 第i为置一 */
            ret = i;
            break;
        } 
    }

    return ret != -1 ? MY_SIGRTMIN + ret : -1;
}


/**
 * delete_signo - 删除信号, 置信号池相应位为0
 * @signo:  需要删除的信号
 * @return: 无
 */
inline void delete_signo(int signo)
{
    signo_pool &= ~((typeof(signo_pool))1 << (signo - MY_SIGRTMIN));
}
