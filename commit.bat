@echo off

echo ==== BetterEdit ====
echo.

git add --all
git commit -a
git push origin master

rem one of these will fail and the other'll work for me lol

cd K:\gdmake\bin\Debug\net5.0\win10-x64\submodules
cd C:\Users\HJfod\Documents\github\GDMake\bin\Debug\net5.0\win10-x64\submodules

echo.
echo ==== gd.h ====
echo.

cd gd.h

git add --all
git commit -a
git push origin main

echo.
echo ==== Cocos2d ====
echo.

cd ..\cocos2d

git add --all
git commit -a
git push origin master

echo.
echo ==== GDNodes ====
echo.

cd ..\nodes

git add --all
git commit -a
git push origin master
