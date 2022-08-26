# Project: SrtSync

CC   = gcc.exe -s
WINDRES = windres.exe
RES  = main.res
OBJ  = main.o $(RES)
LINKOBJ  = main.o $(RES)
LIBS =  -L"C:/Dev-Cpp/lib" -mwindows  
INCS =  -I"C:/Dev-Cpp/include" 
BIN  = SrtSync.exe
CFLAGS = $(INCS)  
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before SrtSync.exe all-after


clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LINKOBJ) -o "SrtSync.exe" $(LIBS)

main.o: main.c
	$(CC) -c main.c -o main.o $(CFLAGS)

main.res: main.rc 
	$(WINDRES) -i main.rc --input-format=rc -o main.res -O coff 

