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

# Platform detection
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	PLATFORM := macos
	PKG_INSTALL := brew install
else ifeq ($(UNAME_S),Linux)
	PLATFORM := linux
	PKG_INSTALL := sudo apt-get install -y
else
	PLATFORM := unsupported
endif

CURDIR := $(shell pwd)

# Variables
NAME		:= streamshot
HDR_DIR		:= $(CURDIR)/include
SRC_DIR		:= $(CURDIR)/src
TESTS_DIR	:= $(CURDIR)/tests

# Platform-specific directories
ifeq ($(PLATFORM),macos)
	LIB_DIR := $(CURDIR)/lib/macos
	BUILD_DIR	:= $(CURDIR)/build/macos
else ifeq ($(PLATFORM),linux)
	LIB_DIR := $(CURDIR)/lib/linux
	BUILD_DIR	:= $(CURDIR)/build/linux
endif

SRCS		:= $(shell find $(SRC_DIR) -type f -name '*.c')
OBJS		:= $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# Library directories
ZLIB_DIR	:= $(LIB_DIR)/zlib
PNG_DIR		:= $(LIB_DIR)/png
JPEG_DIR	:= $(LIB_DIR)/jpeg
FFMPEG_DIR	:= $(LIB_DIR)/ffmpeg

# Compiler and flags
INCLUDES	:= -I$(HDR_DIR) \
			   -I$(ZLIB_DIR)/include \
			   -I$(PNG_DIR)/include \
			   -I$(JPEG_DIR)/include \
			   -I$(FFMPEG_DIR)/include

LDFLAGS		:= -L$(ZLIB_DIR)/lib -lz \
			   -L$(PNG_DIR)/lib -lpng16 \
			   -L$(JPEG_DIR)/lib -ljpeg \
			   -L$(FFMPEG_DIR)/lib -lavformat -lavutil -lavcodec -lswscale

CFLAGS		:= -std=c11 -O2 -DNDEBUG \
               -Wall -Wextra -Werror \
               -fstack-protector-strong -D_FORTIFY_SOURCE=2 \
               -march=native -flto -funroll-loops \
               $(INCLUDES)

DEV_CFLAGS  := -std=c11 -Wall -Wextra -Wpedantic -Werror -O0 -g \
               -fsanitize=address,undefined,signed-integer-overflow,pointer-compare,pointer-subtract,alignment \
               -fno-omit-frame-pointer -fstack-protector-strong \
               $(INCLUDES)

# Platform-specific flags
ifeq ($(PLATFORM),macos)
	CFLAGS += -Wno-error=visibility
	DEV_CFLAGS += -Wno-error=visibility
else ifeq ($(PLATFORM),linux)
	CFLAGS += -static -Wno-error=unused-result
	DEV_CFLAGS += -static -Wno-error=unused-result
endif

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
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "$(YELLOW)Compiling $<...$(NC)"
	$(CC) $(CFLAGS) -c $< -o $@

dev: CFLAGS := $(DEV_CFLAGS) 
dev: clean build_check $(NAME)
	@echo "$(GREEN)Development build complete with sanitizers enabled.$(NC)"

clean:
	@rm -rf $(BUILD_DIR)
	@rm -f $(NAME)
	@rm -rf debug_files
	@echo "$(GREEN)Cleaned build artifacts.$(NC)"

fclean: clean
	@rm -rf $(LIB_DIR)
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
				check_lib_exists \
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

check_lib_exists:
	@if [ ! -d $(LIB_DIR) ]; then \
		mkdir -p $(LIB_DIR); \
	fi

check_build_exists:
	@if [ ! -d $(BUILD_DIR) ]; then \
		mkdir -p $(BUILD_DIR); \
	fi

check_zlib_exists:
	@if [ ! -d $(ZLIB_DIR) ]; then \
		echo "$(YELLOW)Installing zlib (static build for production)...$(NC)"; \
		cd $(LIB_DIR) && \
		curl -LO https://zlib.net/zlib-1.3.1.tar.gz && \
		tar xzf zlib-1.3.1.tar.gz && \
		mv zlib-1.3.1 tmp && \
		cd tmp && \
		./configure --static --prefix="$(ZLIB_DIR)" && \
		make clean && \
		make -j2 && \
		make install && \
		cd .. && \
		rm -rf tmp zlib-1.3.1.tar.gz; \
		echo "$(GREEN)zlib static library installed successfully.$(NC)"; \
	else \
		echo "$(GREEN)zlib exists.$(NC)"; \
	fi

check_png_exists: check_zlib_exists
	@if [ ! -d $(PNG_DIR) ]; then \
		echo "$(YELLOW)Installing libpng (static build for production)...$(NC)"; \
		cd $(LIB_DIR) && \
		curl -LO https://download.sourceforge.net/libpng/libpng-1.6.49.tar.gz && \
		tar xzf libpng-1.6.49.tar.gz && \
		mv libpng-1.6.49 tmp && \
		cd tmp && \
		./configure --disable-shared --enable-static --with-zlib-prefix="$(ZLIB_DIR)" --prefix="$(PNG_DIR)" && \
		make && \
		make install; \
		cd .. && \
		rm -rf tmp libpng-1.6.49.tar.gz; \
		echo "$(GREEN)libpng static library installed successfully.$(NC)"; \
	else \
		echo "$(GREEN)libpng exists.$(NC)"; \
	fi

check_jpeg_exists:
	@if [ ! -d $(JPEG_DIR) ]; then \
		echo "$(YELLOW)Installing libjpeg (static build for production)...$(NC)"; \
		cd $(LIB_DIR) && \
		curl -LO https://ijg.org/files/jpegsrc.v9f.tar.gz && \
		tar xzf jpegsrc.v9f.tar.gz && \
		mv jpeg-9f tmp && \
		cd tmp && \
		./configure --disable-shared --enable-static --prefix="$(JPEG_DIR)" && \
		make && \
		make install; \
		cd .. && \
		rm -rf tmp jpegsrc.v9f.tar.gz; \
		echo "$(GREEN)libjpeg static library installed successfully.$(NC)"; \
	else \
		echo "$(GREEN)libjpeg exists.$(NC)"; \
	fi


check_nasm_exists:
	@if ! command -v nasm >/dev/null 2>&1; then \
		unameOut=$$(uname); \
		if [ "$$unameOut" = "Darwin" ]; then \
			echo "$(YELLOW)Installing nasm via brew...$(NC)"; \
			$(PKG_INSTALL) nasm; \
		elif [ -f /etc/debian_version ]; then \
			echo "$(YELLOW)Installing nasm via apt-get...$(NC)"; \
			$(PKG_INSTALL) nasm; \
		else \
			echo "Please install nasm manually for your OS."; \
			exit 1; \
		fi \
	fi

check_ffmpeg_exists: check_nasm_exists
	@if [ ! -d $(FFMPEG_DIR) ]; then \
		git clone https://git.ffmpeg.org/ffmpeg.git tmp && \
		cd tmp && \
		if [ "$(PLATFORM)" = "linux" ]; then \
			./configure \
				--prefix=$(FFMPEG_DIR) \
				--pkg-config-flags="--static" \
				--extra-cflags="-fPIC" \
				--extra-ldflags="-static" \
				--disable-shared \
				--disable-debug \
				--disable-doc \
				--disable-ffplay \
				--disable-ffprobe \
				--disable-avdevice \
				--enable-static \
				--enable-gpl \
				--enable-version3 \
				--enable-decoder=vp9 ; \
		else \
			./configure \
				--prefix=$(FFMPEG_DIR) \
				--extra-cflags="-fPIC" \
				--disable-shared \
				--disable-debug \
				--disable-doc \
				--disable-ffplay \
				--disable-ffprobe \
				--disable-avdevice \
				--enable-static \
				--enable-gpl \
				--enable-version3 \
				--enable-decoder=vp9 ; \
		fi && \
		make -j3 && \
		make install && \
		cd .. && \
		rm -rf tmp; \
		echo "$(GREEN)FFmpeg static library installed successfully.$(NC)"; \
	else \
		echo "$(GREEN)FFmpeg exists.$(NC)"; \
	fi