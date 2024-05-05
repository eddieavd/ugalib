# ugalib

PROJECT = ugalib
SOURCES = src/core/uga_err.c src/core/uga_log.c src/core/uga_fs.c src/net/uga_sock.c src/net/uga_talk.c src/core/uga_alloc.c src/core/uga_str.c src/core/uga_cli.c src/core/uga_strview.c src/net/uga_mplex.c src/core/uga_vector.c

# CC  = clang-18
# CXX = clang++-18

# CC  = cc
# CXX = c++

CC = zig cc -target aarch64-linux-gnu

IDIR = ./include
LDIR = ./lib

CFLAGS   = -g -Wall -Wextra -pedantic -Werror -O0 -I$(IDIR)
CXXFLAGS =
CPPFLAGS =
LDFLAGS  = -L$(LDIR) -luga
OBJECTS  = ${SOURCES:.c=.o}

CLEANEXTS = o a

BINDIR  = ./bin
TESTDIR = ./bin/test
LIBFILE = $(LDIR)/libuga.a

INSTALL_LDIR = /usr/lib
INSTALL_IDIR = /usr/include/uga

dir_guard = @mkdir -p $(@D)

.PHONY: all
all: $(LIBFILE) examples mrepro test

.PHONY: libonly
libonly: $(LIBFILE)

$(LIBFILE): $(OBJECTS)
	$(dir_guard)
	ar ru $@ $^
	ranlib $@

.PHONY: mrepro
mrepro: $(LIBFILE)
	mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) mrepro/bot.c    -o bot    $(LDFLAGS)
	$(CC) $(CFLAGS) mrepro/server.c -o server $(LDFLAGS)

.PHONY: examples
examples: $(LIBFILE)
	mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) examples/demo.c   -o $(BINDIR)/demo   $(LDFLAGS)
	$(CC) $(CFLAGS) examples/client.c -o $(BINDIR)/client $(LDFLAGS)
	$(CC) $(CFLAGS) examples/server.c -o $(BINDIR)/server $(LDFLAGS)
	$(CC) $(CFLAGS) examples/files.c  -o $(BINDIR)/files  $(LDFLAGS)
	$(CC) $(CFLAGS) examples/string.c -o $(BINDIR)/string $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: test
test: $(LIBFILE)
	mkdir -p $(TESTDIR)
	$(CC) $(CFLAGS) -I./test test/test.c -o $(TESTDIR)/test $(LDFLAGS)
	./$(TESTDIR)/test

.PHONY: install
install:
	mkdir -p $(INSTALL_LDIR)
	mkdir -p $(INSTALL_IDIR)
	cp -p  $(LIBFILE) $(INSTALL_LDIR)
	cp -rp $(IDIR)/* $(INSTALL_IDIR)

.PHONY: clean
clean:
	for file in $(CLEANEXTS); do rm -f *.$$file; rm -f src/core/*.$$file; rm -f src/net/*.$$file; rm -f lib/*.$$file; rm -f ../lib/*$$file; done
	-rm -f $(BINDIR)/demo $(BINDIR)/client $(BINDIR)/server $(BINDIR)/files $(BINDIR)/string $(TESTDIR)/test bot server

.PHONY: cleanall
cleanall:
	for file in $(CLEANEXTS); do rm -f *.$$file; rm -f src/core/*.$$file; rm -f src/net/*.$$file; rm -f lib/*.$$file; rm -f ../lib/*$$file; done
	-rm -f $(BINDIR)/demo $(BINDIR)/client $(BINDIR)/server $(BINDIR)/files $(BINDIR)/string $(TESTDIR)/test bot server
	-rm -rf $(INSTALL_IDIR)
	-rm -f $(INSTALL_LDIR)/libuga.a