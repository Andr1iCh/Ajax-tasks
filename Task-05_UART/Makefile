CROSS_COMPILE ?=
CC = $(CROSS_COMPILE)gcc
CFLAGS ?= -std=c11 -Wall -Wextra -O2
DEPFLAGS = -MMD -MP
CPPFLAGS += -Iinc

BUILD_DIR := build
BIN_DIR := bin
LOG_DIR := logs

SRCS := $(wildcard src/*.c)
OBJS := $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

TARGET := $(BIN_DIR)/echo_app

.PHONY: all clean help deploy

all: $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p $(BIN_DIR)
	$(CC) $^ -o $@

$(BUILD_DIR)/%.o: src/%.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

clean:
	rm -rf $(BIN_DIR) $(BUILD_DIR) $(LOG_DIR)

deploy: $(TARGET)
	scp $(TARGET) $(TARGET_USER)@$(TARGET_HOST):$(DEPLOY_DIR)/

help:
	@echo "make [CROSS_COMPILE=...]"
	@echo "make deploy - requires local.mk (see README.md)"

-include $(DEPS)
-include local.mk