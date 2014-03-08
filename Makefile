SRC := $(shell find src/lib -name '*.c')
BIN := pwm-del pwm-gen pwm-get pwm-list pwm-log pwm-set
PWM := pwm

CFLAGS += -Wall -Isrc/lib -lgpgme -lgit2

.PHONY: all
all: $(PWM) $(BIN)

$(PWM): src/pwm.c
	$(CC) $(CFLAGS) $^ -o $@

%: src/%.c $(SRC)
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f $(PWM) $(BIN)
