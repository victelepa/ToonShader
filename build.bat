@echo off
setlocal
set CXX=cl
set CXXFLAGS=/std:c++17 /O2 /EHsc
%CXX% %CXXFLAGS% /Fe:toonobj.exe main.cpp
echo Built toonobj.exe
