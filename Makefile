CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -std=c99 -pedantic
SRCDIR = src
INCDIR = include
BUILDDIR = build
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)
TARGET = $(BUILDDIR)/proxy

CFLAGS += -Ilib/picohttpparser
SRCS = src/main.c lib/picohttpparser/picohttpparser.c

.PHONY: all clean run

all: $(TARGET)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILDDIR)

run: $(TARGET)
	./$(TARGET)