CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -std=c99 -pedantic `pkg-config --cflags glib-2.0`
SRCDIR = src
INCDIR = include
BUILDDIR = build

SOURCES = $(wildcard $(SRCDIR)/*.c) lib/picohttpparser/picohttpparser.c
OBJECTS = $(SOURCES:%.c=$(BUILDDIR)/%.o)
TARGET = $(BUILDDIR)/proxy

CFLAGS += -Ilib/picohttpparser
LDFLAGS = `pkg-config --libs glib-2.0`

.PHONY: all clean run valgrind

all: $(TARGET)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILDDIR)/%.o: %.c | $(BUILDDIR)
	@mkdir -p $(dir $@) 
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILDDIR)

run: $(TARGET)
	./$(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./$(TARGET)