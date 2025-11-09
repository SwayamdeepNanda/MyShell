.PHONY: all clean format test valgrind release debug install uninstall

NAME = myshell
PREFIX ?= /usr/local
SRCDIR = src
INCDIR = include
BINDIR = bin

SRC = $(wildcard $(SRCDIR)/*.c)
OBJ = $(SRC:.c=.o)

CFLAGS ?= -std=gnu11 -Wall -Wextra -Wpedantic -fstack-protector-strong -D_GNU_SOURCE -I$(INCDIR)
LDFLAGS ?= -ldl

# different build types
debug: CFLAGS += -g -O0
debug: LDFLAGS +=
debug: all

release: CFLAGS += -O3 -DNDEBUG
release: all

all: $(BINDIR)
	$(CC) $(CFLAGS) $(SRC) -o $(BINDIR)/$(NAME) $(LDFLAGS)
	@echo "Built $(BINDIR)/$(NAME)"

$(BINDIR):
	mkdir -p $(BINDIR)

format:
	clang-format -i $(SRC) $(INCDIR)/*.h || true

valgrind:
	@./scripts/run_valgrind.sh

test:
	@echo "Running unit tests..."
	@if [ -f tests/unit_tests.c ]; then \
	  $(CC) $(CFLAGS) tests/unit_tests.c -o tests/unit_tests; \
	  ./tests/unit_tests; \
	else \
	  echo "No unit tests present."; \
	fi

install: release
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	install -m 755 $(BINDIR)/$(NAME) $(DESTDIR)$(PREFIX)/bin/$(NAME)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(NAME)

clean:
	rm -rf $(BINDIR) tests/unit_tests *.o *.gcda *.gcno *.so build
