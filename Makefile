CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -std=c99 -pedantic
SRCDIR = src
INCDIR = include
BUILDDIR = build

SOURCES = $(wildcard $(SRCDIR)/*.c) lib/picohttpparser/picohttpparser.c
OBJECTS = $(SOURCES:%.c=$(BUILDDIR)/%.o)
TARGET = $(BUILDDIR)/proxy

CFLAGS += -Ilib/picohttpparser

.PHONY: all clean run valgrind

all: $(TARGET)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILDDIR)/%.o: %.c | $(BUILDDIR)
	@mkdir -p $(dir $@) 
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILDDIR)

run: $(TARGET)
	./$(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./$(TARGET)