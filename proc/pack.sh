#!/bin/sh  
exe="proc" #你需要发布的程序名称
des="/home/jan/code/OS/lab4/proc" #创建文件夹的位置
deplist=$(ldd $exe | awk  '{if (match($3,"/")){ printf("%s "),$3 } }')  
cp $deplist $des

