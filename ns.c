#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/wait.h>

#include <stdio.h>
#include <sched.h>

#include <signal.h>
#include <unistd.h>

#define STACK_SIZE (1024*1024)

static char child_stack[STACK_SIZE];
int checkpoint[2];

char * const child_args[]={
    "/bin/bash",
    NULL,
};

const char hostname[]="container";

void set_uid_map(pid_t pid, int inside_id, int outside_id, int length) {
    char path[256];
    sprintf(path, "/proc/%d/uid_map", getpid());
    FILE* uid_map = fopen(path, "w");
    fprintf(uid_map, "%d %d %d", inside_id, outside_id, length);
    fclose(uid_map);
}
void set_gid_map(pid_t pid, int inside_id, int outside_id, int length) {
    char path[256];
    sprintf(path, "/proc/%d/gid_map", getpid());
    FILE* gid_map = fopen(path, "w");
    fprintf(gid_map, "%d %d %d", inside_id, outside_id, length);
    fclose(gid_map);
}

int child_main(void *args) {
    char c;
    close(checkpoint[1]);
    printf("world! hostname len %lu\n",sizeof(hostname));
    //设置container的uts
    sethostname (hostname,sizeof(hostname));

    //验证PID隔离
    printf("container Pid [%d]\n", getpid());

    //挂载proc，支持ps top等工具的精确显示
    mount("proc", "/proc", "proc", 0, NULL);

    //等待父进程设置网络完成
    read(checkpoint[0], &c, 1);

    //设置用户的uid
    set_uid_map(getpid(), 0, 0, 1);
    set_gid_map(getpid(), 0, 0, 1);

    //network, 直接可以ping通10.0.1.2
    system("ip link set lo up");
    system("ip link set veth1 up");
    system("ip addr add 10.0.1.2/24 dev veth1");

    execv(child_args[0],child_args);
    printf("Ooops\n");
    return 1;
}
int main() {

    pipe(checkpoint);
    printf ("\n hello?\n");

    system("ip netns add demo");
    int child_pid = clone(child_main,child_stack + STACK_SIZE,
       CLONE_NEWUSER | CLONE_NEWNET |  CLONE_NEWNS | CLONE_NEWPID | CLONE_NEWUTS|SIGCHLD,NULL);

    char *cmd;
    //system("ip netns add %d",child_pid);
    asprintf(&cmd,"ip link set veth1 netns %d",child_pid);
    system("ip link add veth0 type veth peer name veth1");
    system(cmd);
    system("ip link set veth0 up");
    system("ip addr add 10.0.1.3/24 dev veth0");
    free(cmd);
    close(checkpoint[1]);
    waitpid(child_pid,NULL,0);    
    system("ip netns delete demo");
    return 0;
}
