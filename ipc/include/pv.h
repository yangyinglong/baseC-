#ifndef __PV_H__
#define __PV_H__

// 初始化semnums个信号灯/信号量的值(value)
extern int I(int semnums, int value);

// 对信号量集(semid)中的信号灯(semnum)作P(value)操作
extern void P(int semid, int semnum, int value);

// 对信号量集(semid)中的信号灯(semnum)做V(value)操作
extern void V(int semid, int semnum, int value);

// 销毁信号量集(semid)
extern void D(int semid);

#endif