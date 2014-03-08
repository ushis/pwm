SRC := $(shell find src/lib -name '*.c')
BIN := pwm-del pwm-gen pwm-get pwm-list pwm-set

CFLAGS += -Wall -Isrc/lib -lgpgme -lgit2

.PHONY: all
all: $(BIN)

%: src/%.c $(SRC)
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f $(BIN)
