# MultiWork
multiple thead work test

//降低多线程程序对全局变量的"写" 是重点
//多线程的写操作一旦进入公有区域 不管这个内容有没有竞争 都会使运行速度大为下降
//最好的办法是定义一个局部的“结果数据” 把运算内容放在这个“结果数据”内 才能发挥多线程的最大功效
//公有区域的读操作是没有任何问题的