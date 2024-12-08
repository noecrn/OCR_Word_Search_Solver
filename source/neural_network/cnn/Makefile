CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lm -lSDL2 -lSDL2_image -lpng
TARGET = train 
SRCS = train_data.c debug.c init.c log.c read_data.c read_init.c read_png.c train.c write_data.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

# Format all source files in this directory
format:
	@if ls *.c >/dev/null 2>&1; then clang-format -i *.c; fi
	@if ls *.h >/dev/null 2>&1; then clang-format -i *.h; fi

.PHONY: all clean format