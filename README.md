目录结构如下：
work@agent:~$ tree mysh/
mysh/
├── ~
├── mypipe.c
├── mypipo
├── netunshare
├── netunshare.c
└── README.md

创建步骤：
1， 首先编译netunshare.c，用unshare来创建一个虚拟网络环境
2,  创建管道，跟虚拟环境进行通信
3， 继续完善轻量虚拟机

参考:
[Linux 上实现双向进程间通信管道](http://www.ibm.com/developerworks/cn/linux/l-pipebid/)

[用unshare创建轻量级虚拟环境] (http://lidemin.pw/226)    
