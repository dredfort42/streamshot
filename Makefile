/*******************************************************************

		::          ::        +--------+-----------------------+
		  ::      ::          | Author | Dmitry Novikov        |
		::::::::::::::        | Email  | dredfort.42@gmail.com |
	  ::::  ::::::  ::::      +--------+-----------------------+
	::::::::::::::::::::::
	::  ::::::::::::::  ::    File     | Makefile
	::  ::          ::  ::    Created  | 2025-06-05
		  ::::  ::::          Modified | 2025-06-05

	GitHub:   https://github.com/dredfort42
	LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

# Variables
NAME    := streamshot
SRCS    := $(wildcard src/*.c)
BUILD   := build
CC      := gcc
OBJS    := $(patsubst src/%.c,$(BUILD)/%.o,$(SRCS))
HDRS  := include
CFLAGS  := -Wall -Wextra -Werror -I$(HDRS)

# Rules
.PHONY: all build clean fclean re

all: check_src_exists check_build_exists build

check_src_exists:
	@if [ ! -d src ]; then \
		echo "Error: src directory does not exist."; \
		exit 1; \
	fi

check_build_exists:
	@if [ ! -d $(BUILD) ]; then \
		mkdir -p $(BUILD); \
	fi

build: check_build_exists $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD)/%.o: src/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(BUILD)

fclean: clean
	@rm -rf debug_files
	@rm -f $(NAME)

re: fclean all