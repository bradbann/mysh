#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
struct ProcessInfo{
    FILE* stream; ///< Pointer to the duplex pipe stream
    pid_t pid; ///< Process ID of the command
};
int dphalfclose(FILE *stream)
{
    /* Ensure all data are flushed */
    if (fflush(stream) == EOF)
        return -1;
    /* Close pipe for writing */
    return shutdown(fileno(stream), SHUT_WR);
}
struct ProcessInfo dpopen(char *const command[])
{
        int fd[2];
        pid_t pid;
        struct ProcessInfo pi;
        FILE *stream;
        // 创建管道
        socketpair(AF_UNIX, SOCK_STREAM, 0, fd);
        if ( (pid = fork()) == 0) { // 子进程
            // 关闭管道的父进程端
            close(fd[0]);
            // 复制管道的子进程端到标准输出
            dup2(fd[1], STDOUT_FILENO); 
            // 复制管道的子进程端到标准输入
            dup2(fd[1], STDIN_FILENO);
            // 关闭已复制的读管道
            close(fd[1]);
            /* 使用exec执行命令 */
            execvp(command[0],command);
        } else {    // 父进程
            // 关闭管道的子进程端
            close(fd[1]);
            stream = fdopen(fd[0], "r+");
            if (stream == NULL) {
                close(fd[0]);
                return;
            }
            /* Successfully return here */
            pi.pid = pid;
            pi.stream = stream;
            return pi;
        }
}

int main() {
    char *cmd[] = {"/home/work/mysh/netunshare","/bin/bash"};
    struct ProcessInfo pi = dpopen(cmd);	
    char buf[10240];
    char icmd[256];
    int n;
    printf("child pid : %d\n",pi.pid);
    fprintf(pi.stream,"pwd");
    fflush(pi.stream);
    dphalfclose(pi.stream);
    n=fscanf(pi.stream,"%s",buf);
    printf("read : %d == %s\n",n,buf);
            
    return 0;
}
