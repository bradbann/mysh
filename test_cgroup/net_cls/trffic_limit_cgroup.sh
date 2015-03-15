#!/bin/bash


if [ $# -lt 2 ] ; then
    echo "Parameter error"
    echo "./trffic_limit_cgroup.sh dev limit"
    exit
fi

DEV=$1
limit=$2

tc qdisc del dev $DEV root

tc qdisc add dev $DEV root handle 1: htb
tc class add dev $DEV parent 1: classid 1: htb rate 1000mbit ceil 1000mbit
tc class add dev $DEV parent 1: classid 1:2 htb rate ${limit}mbit

tc filter add dev $DEV protocol ip parent 1:0 prio 1 handle 1:2 cgroup
