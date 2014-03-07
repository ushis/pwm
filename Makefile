SRC := $(shell find src/lib -name '*.c')
BIN := pwm-get pwm-set pwm-del pwm-list

.PHONY: all
all: $(BIN)

%: src/%.c $(SRC)
	$(CC) $(CFLAGS) -Wall -Isrc/lib -lgpgme -lgit2 $^ -o $@

.PHONY: clean
clean:
	rm -f $(BIN)
