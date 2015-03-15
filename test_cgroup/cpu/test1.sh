#!/bin/bash

set -x

root_cgroup="/sys/fs/cgroup"

function _clean() {

    cgs=(test1 test2)
    for cg in ${cgs[@]}
    do
        //冻结进程
        while read pid
        do
           kill -19 $pid
        done < $root_cgroup/cpu/$cg/tasks

    done

    sleep 1


    for cg in ${cgs[@]}
    do
        //删除进程
        while read pid
        do
           kill -9 $pid
        done < $root_cgroup/cpu/$cg/tasks
        cgdelete cpu:$cg

    done

}

trap _clean INT

cgcreate -g cpu:/test1
cgcreate -g cpu:/test2


#设置不同组cpu的使用时间的相对配额
cgset -r cpu.shares=512 test1
cgset -r cpu.shares=128 test2

# 设置test1的cpu使用时间为50%,默认cpu.cfs_period=100000us
#cgset -r cpu.cfs_quota_us=50000 test1
#cgset -r cpu.cfs_quota_us=25000 test2

loop=0

while (($loop < 4))
do
    cgexec -g cpu:test1 ./deadloop > /dev/null &
    ((loop++))
done

while (($loop < 4 + 4))
do
    cgexec -g cpu:test2 ./deadloop2 > /dev/null &
    ((loop++))
done

echo "done"
sleep 10000
