del "main.o" /f /q /a:-
del "main.res" /f /q /a:-
set PATH=%PATH%;C:\Dev-Cpp\bin;
make -f "Makefile"
pause
