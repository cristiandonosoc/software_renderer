@echo off
pushd .\build

REM CLEAN
del *.pdb *.obj *.ilk *.exe

SET WINDOWS_LIBS=gdi32.lib user32.lib
SET FLAGS=/EHsc /Od /MTd /Zi
SET OUTPUT=/Ferenderer.exe
cl ..\win32_main.c %WINDOWS_LIBS% %FLAGS% %OUTPUT%

popd
