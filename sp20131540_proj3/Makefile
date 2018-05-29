MACRO = 20131540.o function.o
CC = gcc
CFLAGS = -Wall 
TARGET = 20131540.out

all : $(TARGET)


$(TARGET): $(MACRO)
	$(CC) -o $(TARGET) $(MACRO)

clean :
	rm -rf $(MACRO) $(TARGET)
20131540.o : 20131540.h 20131540.c
function.o : 20131540.h function.c
