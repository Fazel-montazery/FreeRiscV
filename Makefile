SRC := src/main.c src/cpu.c src/ram.c src/fs.c src/bus.c src/env.c
CC := gcc
TARGET := frv
FLAGS_RELEASE := -Wall -O2 -std=c99
FLAGS_DEBUG := -Wall -g3 -fsanitize=address -std=c99

all: main

main: ${SRC}
	${CC} -o ${TARGET} ${SRC} ${FLAGS_RELEASE}

debug: ${SRC}
	${CC} -o ${TARGET} ${SRC} ${FLAGS_DEBUG}

run:
	./${TARGET}

clean:
	rm ${TARGET}
