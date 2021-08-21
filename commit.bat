@echo off

echo ==== BetterEdit ====
echo.

git add --all
git commit -a
git push origin master

cd K:\gdmake\bin\Debug\net5.0\win10-x64\submodules

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
