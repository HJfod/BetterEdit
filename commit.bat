@echo off

git add --all
git commit -a
git push origin master

cd K:\gdmake\bin\Debug\net5.0\win10-x64\submodules

cd gd.h

git add --all
git commit -a
git push origin main

if "%1" neq "cocos" goto done

cd ..\cocos2d

git add --all
git commit -a
git push origin main

:done
