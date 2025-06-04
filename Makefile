# Variables
CC      := gcc
CFLAGS  := -Wall -Wextra -Werror
NAME    := streamshot
SRCS    := $(wildcard src/*.c)
BUILD   := build
OBJS    := $(patsubst src/%.c,$(BUILD)/%.o,$(SRCS))

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
	rm -rf $(BUILD)

fclean: clean
	rm -f $(NAME)

re: fclean all