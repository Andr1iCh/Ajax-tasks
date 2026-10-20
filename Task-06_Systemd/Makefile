CROSS_COMPILE ?=
CC = $(CROSS_COMPILE)gcc
CFLAGS ?= -std=c11 -Wall -Wextra -O2
DEPFLAGS = -MMD -MP
CPPFLAGS += -Iinc

UART_DEVICE ?= /dev/ttyAMA0
INSTALL_BIN_DIR ?= /usr/local/bin

BUILD_DIR := build
BIN_DIR := bin
LOG_DIR := logs

SRCS := $(wildcard src/*.c)
OBJS := $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

TARGET := $(BIN_DIR)/echo_app

.PHONY: all clean help deploy install uninstall

all: $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: src/%.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

clean:
	rm -rf $(BIN_DIR) $(BUILD_DIR)

deploy: $(TARGET)
	scp $(TARGET) $(TARGET_USER)@$(TARGET_HOST):$(DEPLOY_DIR)/

install: $(TARGET) uart-echo.service
	scp "$(TARGET)" uart-echo.service "$(TARGET_USER)@$(TARGET_HOST):$(DEPLOY_DIR)/"
	ssh -t "$(TARGET_USER)@$(TARGET_HOST)" "\
		sudo install -D -m 0755 '$(DEPLOY_DIR)/echo_app' '$(INSTALL_BIN_DIR)/echo_app' && \
		sudo install -D -m 0644 '$(DEPLOY_DIR)/uart-echo.service' /etc/systemd/system/uart-echo.service && \
		sudo sed -i -e 's|^User=.*|User=$(TARGET_USER)|' \
		    -e 's|/usr/local/bin/echo_app|$(INSTALL_BIN_DIR)/echo_app|' /etc/systemd/system/uart-echo.service && \
		printf 'UART_DEVICE=%s\n' '$(UART_DEVICE)' | sudo tee /etc/uart-echo.conf > /dev/null && \
		sudo systemctl daemon-reload && \
		sudo systemctl enable uart-echo.service"

uninstall:
	ssh -t "$(TARGET_USER)@$(TARGET_HOST)" "\
		sudo systemctl disable --now uart-echo.service && \
		sudo rm -f '$(INSTALL_BIN_DIR)/echo_app' \
		    /etc/systemd/system/uart-echo.service \
		    /etc/uart-echo.conf && \
		rm -f '$(DEPLOY_DIR)/echo_app' \
		    '$(DEPLOY_DIR)/uart-echo.service' && \
		sudo systemctl daemon-reload"
		
help:
	@echo "make [CROSS_COMPILE=...]"
	@echo "make deploy - requires local.mk (see README.md)"

-include $(DEPS)
-include local.mk