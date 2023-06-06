@echo off
setlocal

set cpy=robocopy /NJH /NJS

if exist pkg rmdir /s /q pkg

%CPY% %NWGE_PATH% pkg *.dll
%CPY% target pkg *.exe
%CPY% data pkg\data

cd pkg
7z a -bd -sdel -stl nwge-samples.7z **
cd ..

endlocal
