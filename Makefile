CC = gcc
CFLAGS = -Wall -O3

TAR = MyTar
OBJS = SmartAlloc.o Tar.o Parse.o Create.o Extract.o Table.o

$(TAR) : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TAR)
	@echo "Executable MyTar built"

Tar.o : Tar.c Tar.h
	$(CC) $(CFLAGS) -c Tar.c
	@echo "Tar built"

Parse.o : Parse.c Parse.h Tar.h
	$(CC) $(CFLAGS) -c Parse.c
	@echo "Parse built"

Create.o : Create.c Create.h Tar.h
	$(CC) $(CFLAGS) -c Create.c
	@echo "Create built"

Extract.o : Extract.c Extract.h Tar.h
	$(CC) $(FLAGS) -c Extract.c
	@echo "Extract built"

Table.o : Table.c Table.h Tar.h
	$(CC) $(CFLAGS) -c Table.c
	@echo "Table built"

SmartAlloc.o : SmartAlloc.h SmartAlloc.c
	$(CC) $(CFLAGS) -c SmartAlloc.c
	@echo "SmartAlloc built"

clean : 
	rm $(OBJS) $(TAR)
