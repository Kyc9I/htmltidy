@echo off

REM onetest.cmd - execute a single test case
REM
REM (c) 1998-2003 (W3C) MIT, ERCIM, Keio University
REM See tidy.c for the copyright notice.
REM
REM <URL:http://tidy.sourceforge.net/>
REM
REM CVS Info:
REM
REM    %Author: creitzel %
REM    %Date: 2003/02/16 19:33:12 %
REM    %Revision: 1.6 %

set VERSION='%Id'

echo Testing %1
set TESTNO=%1
set EXPECTED=%2

set TIDY=..\bin\tidy
set INFILES=.\input\in_%1.*ml
set CFGFILE=.\input\cfg_%1.txt

set TIDYFILE=.\tmp\out_%1.html
set MSGFILE=.\tmp\msg_%1.txt

set HTML_TIDY=

REM If no test specific config file, use default.
if NOT exist %CFGFILE% set CFGFILE=.\input\cfg_default.txt

REM Get specific input file name
for %%F in ( %INFILES% ) do set INFILE=%%F 

REM $T set TIDYFILE=.\tmp\out_%1%%~xF

REM Remove any pre-exising test outputs
if exist %MSGFILE%  rm %MSGFILE%
if exist %TIDYFILE% rm %TIDYFILE%

%TIDY% -f %MSGFILE% -config %CFGFILE% %3 %4 %5 %6 %7 %8 %9 --tidy-mark no -o %TIDYFILE% %INFILE%
set STATUS=%ERRORLEVEL%

if %STATUS% EQU %EXPECTED% goto done
cat %MSGFILE

:done