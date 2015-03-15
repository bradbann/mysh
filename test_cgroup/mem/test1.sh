#!/bin/bash

set -x

root_cgroup="/sys/fs/cgroup"

opt=memory
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

_mem_create

cgexec -g memory:test1 ./mem_alloc
cgexec -g memory:test2 dd if=/dev/zero of=./file.256M bs=1M count=512


sleep 10000
