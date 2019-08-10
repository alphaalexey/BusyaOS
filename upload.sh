#!/bin/sh

mkdir out
cp ./disk.img out/
cd out

git config --global user.name "alphaalexey"
git config --global user.email "homyakovost@gmail.com"

git init
git add disk.img
git commit -m "Output"
git remote add origin git@github.com:alphaalexey/BusyaOS-out.git
git push -u origin master