@echo off

REM alltest.cmd - execute all test cases
REM
REM (c) 1998-2006 (W3C) MIT, ERCIM, Keio University
REM See tidy.c for the copyright notice.
REM
REM <URL:http://tidy.sourceforge.net/>
REM
REM CVS Info:
REM
REM    $Author$
REM    $Date$
REM    $Revision$

REM (for MS compiler users):
REM call alltest1 ..\build\msvc\Release\tidy.exe .\tmp

if exist ..\bin\tidy_dbg.exe goto debugmode

call alltest1 ..\bin\tidy.exe .\tmp

goto ende

:debugmode

call alltest1 ..\bin\tidy_dbg.exe .\tmp

:ende

pause
