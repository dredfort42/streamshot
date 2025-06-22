######################################################################
#                                                                    #
#        ::          ::        +--------+-----------------------+    #
#          ::      ::          | Author | Dmitry Novikov        |    #
#        ::::::::::::::        | Email  | dredfort.42@gmail.com |    #
#      ::::  ::::::  ::::      +--------+-----------------------+    #
#    ::::::::::::::::::::::                                          #
#    ::  ::::::::::::::  ::    File     | Makefile                   #
#    ::  ::          ::  ::    Created  | 2025-06-05                 #
#          ::::  ::::          Modified | 2025-06-21                 #
#                                                                    #
#    GitHub:   https://github.com/dredfort42                         #
#    LinkedIn: https://linkedin.com/in/novikov-da                    #
#                                                                    #
######################################################################

MACOS := macos
LINUX := linux

# Platform detection
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	PLATFORM := $(MACOS)
	PKG_INSTALL := brew install
	N_CPU := $(shell sysctl -n hw.ncpu)
else ifeq ($(UNAME_S),Linux)
	PLATFORM := $(LINUX)
	PKG_INSTALL := sudo apt-get install -y
	N_CPU := $(shell nproc)
else
	PLATFORM := unsupported
endif

CRNT_DIR := $(shell pwd)

# Variables
NAME		:= streamshot
HDR_DIR		:= $(CRNT_DIR)/include
SRC_DIR		:= $(CRNT_DIR)/src
TESTS_DIR	:= $(CRNT_DIR)/tests

# Platform-specific directories
ifeq ($(PLATFORM),$(MACOS))
	LIB_DIR := $(CRNT_DIR)/lib/macos
	BUILD_DIR	:= $(CRNT_DIR)/build/macos
else ifeq ($(PLATFORM),$(LINUX))
	LIB_DIR := $(CRNT_DIR)/lib/linux
	BUILD_DIR	:= $(CRNT_DIR)/build/linux
endif

SRCS		:= $(shell find $(SRC_DIR) -type f -name '*.c')
OBJS		:= $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# Library directories
ZLIB_DIR	:= $(LIB_DIR)/zlib
PNG_DIR		:= $(LIB_DIR)/png
JPEG_DIR	:= $(LIB_DIR)/jpeg
FFMPEG_DIR	:= $(LIB_DIR)/ffmpeg

# Compiler and flags
INCLUDES	:= -I$(HDR_DIR)

# Platform-specific flags
ifeq ($(PLATFORM),$(MACOS))
	CFLAGS += -Wno-error=visibility
	DEV_CFLAGS += -Wno-error=visibility
	INCLUDES += -I/usr/local/opt/ffmpeg/include -I/usr/local/opt/libjpeg/include -I/usr/local/opt/libpng/include
	LDFLAGS := -L/usr/local/opt/ffmpeg/lib -L/usr/local/opt/libjpeg/lib -L/usr/local/opt/libpng/lib
else ifeq ($(PLATFORM),$(LINUX))
	CFLAGS += -static -Wno-error=unused-result
	DEV_CFLAGS += -static -Wno-error=unused-result
endif

CFLAGS		:= -std=c11 -O2 -DNDEBUG \
               -Wall -Wextra -Werror \
               -fstack-protector-strong -D_FORTIFY_SOURCE=2 \
               -march=native -flto -funroll-loops \
               $(INCLUDES)

DEV_CFLAGS  := -std=c11 -Wall -Wextra -Wpedantic -Werror -O0 -g \
               -fsanitize=address,undefined,signed-integer-overflow,pointer-compare,pointer-subtract,alignment \
               -fno-omit-frame-pointer -fstack-protector-strong \
               $(INCLUDES)

LIBS		:= -lpng -ljpeg -lavformat -lavcodec -lavutil -lswscale -lm -lz -pthread

CC		  := gcc

# Output colors
GREEN	:= \033[0;32m
RED		:= \033[0;31m
YELLOW	:= \033[1;33m
NC		:= \033[0m

# Rules
.PHONY: all build dev clean fclean re test help

all: build

build: build_check $(NAME)
	@echo "$(GREEN)Build complete.$(NC)"

$(NAME): $(OBJS)
	@echo "$(YELLOW)Linking $@...$(NC)"
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "$(YELLOW)Compiling $<...$(NC)"
	$(CC) $(CFLAGS) -c $< -o $@

dev: CFLAGS := $(DEV_CFLAGS) 
dev: clean build_check $(NAME)
	@echo "$(GREEN)Development build complete with sanitizers enabled.$(NC)"

clean:
	@rm -rf $(BUILD_DIR)
	@rm -rf debug_files
	@echo "$(GREEN)Cleaned build artifacts.$(NC)"

fclean: clean
	@rm -f $(NAME)
	@rm -rf *.dSYM
	@echo "$(GREEN)Fully cleaned all artifacts and libraries.$(NC)"

re: clean all

help:
	@echo "$(YELLOW)Available targets:$(NC)"
	@echo "  all	  - Build the project (default)"
	@echo "  build	- Build the project"
	@echo "  dev	  - Build the project with sanitizers enabled for development"
	@echo "  clean	- Remove build artifacts"
	@echo "  fclean   - Remove all build artifacts and libraries"
	@echo "  re	   - Clean and rebuild"
	@echo "  test	 - Build and run tests"
	@echo "  help	 - Show this help message"

# Check for existence of directories
build_check: 	check_platform \
				check_src_exists \
				check_build_exists \
				check_png_exists \
				check_jpeg_exists \
				check_ffmpeg_exists
	@echo "$(GREEN)Ready to build.$(NC)"

check_platform:
	@if [ "$(PLATFORM)" = "unsupported" ]; then \
		echo "$(RED)Error: Unsupported platform $(UNAME_S). Only macOS and Linux are supported.$(NC)"; \
		exit 1; \
	else \
		echo "$(YELLOW)Platform detected: $(PLATFORM)$(NC)"; \
	fi

check_src_exists:
	@if [ ! -d src ]; then \
		echo "$(RED)Error: src directory does not exist.$(NC)"; \
		exit 1; \
	fi

check_build_exists:
	@if [ ! -d $(BUILD_DIR) ]; then \
		mkdir -p $(BUILD_DIR); \
	fi

check_png_exists: 
	@echo "$(YELLOW)Checking for libpng...$(NC)"; \
	if [ "$(PLATFORM)" = "$(MACOS)" ]; then \
		if ! brew list libpng >/dev/null 2>&1; then \
			echo "$(YELLOW)libpng not found. Installing...$(NC)"; \
			$(PKG_INSTALL) libpng; \
		else \
			echo "$(GREEN)libpng found via brew.$(NC)"; \
		fi; \
	elif [ "$(PLATFORM)" = "$(LINUX)" ]; then \
		if ! dpkg -s libpng-dev >/dev/null 2>&1; then \
			echo "$(YELLOW)libpng not found. Installing...$(NC)"; \
			$(PKG_INSTALL) libpng-dev; \
		else \
			echo "$(GREEN)libpng-dev found.$(NC)"; \
		fi; \
	else \
		echo "$(RED)Please install libpng manually for your OS.$(NC)"; \
		exit 1; \
	fi

check_jpeg_exists:
	@echo "$(YELLOW)Checking for libjpeg...$(NC)"; \
	if [ "$(PLATFORM)" = "$(MACOS)" ]; then \
		if ! brew list jpeg >/dev/null 2>&1; then \
			echo "$(YELLOW)libjpeg not found. Installing...$(NC)"; \
			$(PKG_INSTALL) jpeg; \
		else \
			echo "$(GREEN)libjpeg found.$(NC)"; \
		fi; \
	elif [ "$(PLATFORM)" = "$(LINUX)" ]; then \
		if ! dpkg -s libjpeg-dev >/dev/null 2>&1; then \
			echo "$(YELLOW)libjpeg not found. Installing...$(NC)"; \
			$(PKG_INSTALL) libjpeg-dev; \
		else \
			echo "$(GREEN)libjpeg-dev found.$(NC)"; \
		fi; \
	else \
		echo "$(RED)Please install libjpeg manually for your OS.$(NC)"; \
		exit 1; \
	fi

check_ffmpeg_exists:
	@echo "$(YELLOW)Checking for ffmpeg...$(NC)"; \
	if ! command -v ffmpeg >/dev/null 2>&1; then \
		if [ "$(PLATFORM)" = "$(MACOS)" ]; then \
			echo "$(YELLOW)ffmpeg not found. Installing...$(NC)"; \
			$(PKG_INSTALL) ffmpeg; \
		elif [ "$(PLATFORM)" = "$(LINUX)" ]; then \
			echo "$(YELLOW)ffmpeg not found. Installing...$(NC)"; \
			$(PKG_INSTALL) ffmpeg; \
		else \
			echo "$(RED)Please install ffmpeg manually for your OS.$(NC)"; \
			exit 1; \
		fi; \
	else \
		echo "$(GREEN)ffmpeg found.$(NC)"; \
	fi