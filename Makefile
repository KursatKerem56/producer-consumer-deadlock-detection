CC=gcc
CFLAGS=-Wall -Wextra -pthread -Iinclude -std=c99
SRC=$(wildcard src/*.c)
OBJ=$(SRC:.c=.o)
OUT=app

all: $(OUT)

$(OUT): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(OUT)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./app configs/experiment1.conf

run1: all
	./app configs/experiment1.conf

run2: all
	./app configs/experiment2.conf

run3: all
	./app configs/experiment3.conf

clean:
	rm -f $(OBJ) $(OUT) simulation.log

.PHONY: all run run1 run2 run3 clean
