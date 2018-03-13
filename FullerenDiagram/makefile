TARGET = fullerne
PREFIX ?= /usr/local
SRCS = main.cpp build.cpp draw.cpp expand.cpp paint.cpp inverseMatrix.cpp
OBJS = $(SRCS:.c=.o)

.PHONY: all clean install uninstall

all: $(TARGET)
$(TARGET): $(OBJS)
            $(CC) -o $(TARGET) $(OBJS) $(CFLAGS)
 
.c.o:
            $(CC) $(CFLAGS)  -c $< -o $@

clean:
            rm -rf $(TARGET) $(OBJS)
install:
            install $(TARGET) $(PREFIX)/bin
uninstall:
            rm -rf $(PREFIX)/bin/$(TARGET)