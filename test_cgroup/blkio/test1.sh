#!/bin/bash

set -x

root_cgroup="/sys/fs/cgroup"

opt=blkio
cgs=(test1 test2)

function _clean() {

    for cg in ${cgs[@]}
    do
        //冻结进程
        while read pid
        do
           kill -19 $pid
        done < $root_cgroup/$opt/$cg/tasks

    done

    sleep 1


    for cg in ${cgs[@]}
    do
        //删除进程
        while read pid
        do
           kill -9 $pid
        done < $root_cgroup/$opt/$cg/tasks
        cgdelete $opt:$cg

    done

}

trap _clean INT

function _cpu_create() {
    for i in ${cgs[@]}
    do
        cgcreate -g cpu:/$i
    done
}

function _mem_create() {
    for i in ${cgs[@]}
    do
        cgcreate -g memory:/$i
        # 限制内存和虚拟内存
        cgset -r memory.limit_in_bytes=16M $i
        cgset -r memory.memsw.limit_in_bytes=16M $i
        # 关闭oom killer
        #cgset -r memory.oom_control=1 $i
    done
}


function _blkio_create() {
    for i in ${cgs[@]}
    do
        cgcreate -g blkio:/$i
    done

}

_blkio_create

cgset -r blkio.weight=200 test1
cgset -r blkio.weight=400 test2
cgset -r blkio.throttle.write_bps_device="253:1 1048576" test1

#注意blkio对page cache无效，所以这只对direct io进行设置

rm -rf ./2g*

cgexec -g blkio:test1 dd if=/dev/sda of=./2g oflag=direct bs=1M count=1014 &
cgexec -g blkio:test2 dd if=/dev/sda of=./2g2 oflag=direct bs=1M count=1024 &


sleep 10000
