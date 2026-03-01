CC = gcc
CFLAGS = -Wall -g
TARGET = out.exe
OBJS = driver.o lexer.o parser.o

# Default input and output files
INPUT ?= sample.moo
OUTPUT ?= output.txt

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

driver.o: driver.c lexer.h parser.h
	$(CC) $(CFLAGS) -c driver.c

lexer.o: lexer.c lexer.h lexerDef.h
	$(CC) $(CFLAGS) -c lexer.c

parser.o: parser.c parser.h parserDef.h lexer.h
	$(CC) $(CFLAGS) -c parser.c

# Run the executable with the specified input and output files
run: $(TARGET)
	./$(TARGET) $(INPUT) $(OUTPUT)

debug: $(TARGET)
	gdb --args ./$(TARGET) $(INPUT) $(OUTPUT)

clean:
	rm -f $(OBJS) $(TARGET)