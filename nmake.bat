@echo off
rem Any copyright is dedicated to the Public Domain.
rem http://creativecommons.org/publicdomain/zero/1.0/
echo "#define CIN_DLL 1" > cin_conf.inc
nmake /nologo /f nmakefile %*
