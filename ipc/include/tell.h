#ifndef __TELL_H__
#define __TELL_H__

// include/tell.h
// extern是计算机语言中的一个关键字，
// 可置于变量或者函数前，以表示变量或者函数的定义在别的文件中。

// 管道初始化
extern void init();

// 利用管道进行等待
extern void wait_pipe();

// 利用管道进行通知
extern void notify_pipe();

// 销毁管道
extern void destroy_pipe();

#endif