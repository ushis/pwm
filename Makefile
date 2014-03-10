LIBSRC := $(wildcard lib/*.c)
LIBOBJ := $(patsubst %.c,%.o,$(LIBSRC))
LIBARC := lib/pwm.a

BUILTINSRC := $(wildcard builtin/*.c)
BUILTINOBJ := $(patsubst %.c,%.o,$(BUILTINSRC))
BUILTINBIN := $(patsubst %.o,%,$(BUILTINOBJ))

SRC := pwm.c
BIN := $(patsubst %.c,%,$(SRC))

CFLAGS += -std=c99 -Wall -O2 -D_GNU_SOURCE -Ilib
LDFLAGS += -lgpgme -lgit2

.PHONY: all
all: $(BIN) $(BUILTINBIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILTINBIN): %: %.o $(LIBARC)
	$(CC) $(LDFLAGS) -o $@ $^

$(BUILTINOBJ): %.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(LIBARC): $(LIBOBJ)
	$(AR) csr $@ $^

$(LIBOBJ): %.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

.PHONY: clean
clean:
	rm -f $(LIBOBJ) $(LIBARC) $(BUILTINOBJ) $(BUILTINBIN) $(BIN)
