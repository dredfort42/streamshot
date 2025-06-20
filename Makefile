######################################################################
#                                                                    #
#        ::          ::        +--------+-----------------------+    #
#          ::      ::          | Author | Dmitry Novikov        |    #
#        ::::::::::::::        | Email  | dredfort.42@gmail.com |    #
#      ::::  ::::::  ::::      +--------+-----------------------+    #
#    ::::::::::::::::::::::                                          #
#    ::  ::::::::::::::  ::    File     | Makefile                   #
#    ::  ::          ::  ::    Created  | 2025-06-05                 #
#          ::::  ::::          Modified | 2025-06-20                 #
#                                                                    #
#    GitHub:   https://github.com/dredfort42                         #
#    LinkedIn: https://linkedin.com/in/novikov-da                    #
#                                                                    #
######################################################################

# Variables
NAME        := streamshot
SRC_DIR     := src
TESTS_DIR  := tests
BUILD_DIR   := build
LIB_DIR     := lib

SRCS        := $(shell find $(SRC_DIR) -type f -name '*.c')

#FFMPEG library
FFMPEG_DIR 	:= ${LIB_DIR}/ffmpeg

#JPEG library
JPEG_DIR    := ${LIB_DIR}/jpeg

#PNG library
PNG_DIR    := ${LIB_DIR}/png

#ZLIB library
ZLIB_DIR    := ${LIB_DIR}/zlib

CC          := gcc
OBJS        := $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SRCS))

HDRS        := include

INCLUDES    := -I$(HDRS) \
               -I$(ZLIB_DIR)/include \
               -I$(PNG_DIR)/include \
               -I$(JPEG_DIR)/include \
               -I$(FFMPEG_DIR)/libavformat -I$(FFMPEG_DIR)/libavutil -I$(FFMPEG_DIR)/libavcodec -I$(FFMPEG_DIR)/libswscale

LDFLAGS     := -L$(ZLIB_DIR)/lib -lz \
               -L$(PNG_DIR)/lib -lpng \
               -L$(JPEG_DIR)/lib -ljpeg \
               -L$(FFMPEG_DIR) -lavformat -lavutil -lavcodec -lswscale


# -O3: Enables high-level optimizations for speed. The compiler tries to make your code run as fast as possible.
# -march=native: Generates code optimized for the architecture of the machine compiling the code. It uses all instruction sets available on your CPU.
# -flto: Enables Link Time Optimization, allowing the compiler to optimize across files during the linking stage for better performance.
# -funroll-loops: Tells the compiler to unroll loops where possible, which can make loops run faster at the cost of larger binaries.
CFLAGS      := -O3 -march=native -flto -funroll-loops -Wall -Wextra -Werror -Wno-error=visibility $(INCLUDES)
# CFLAGS      := -g -O0 -fsanitize=address -Wall -Wextra -Werror -Wno-error=visibility $(INCLUDES)

# Rules
.PHONY: all build clean fclean re test

all: build

build: build_check $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(BUILD_DIR)
	@rm -f $(NAME)
	@rm -rf debug_files

fclean: clean
	@rm -rf $(LIB_DIR)

re: clean all

test: build_check
	@mkdir -p $(BUILD_DIR)/tests
	$(CC) $(CFLAGS) $(wildcard $(TESTS_DIR)/*.c) $(filter-out $(SRC_DIR)/main.c, $(SRCS)) -o $(BUILD_DIR)/tests/test_runner $(LDFLAGS)
	@clear
	@$(BUILD_DIR)/tests/test_runner


# Check for existence of directories
build_check: check_src_exists check_build_exists check_lib_exists check_ffmpeg_exists check_jpeg_exists check_zlib_exists check_png_exists
	@echo "All necessary directories exist."

check_src_exists:
	@if [ ! -d src ]; then \
		echo "Error: src directory does not exist."; \
		exit 1; \
	fi

check_lib_exists:
	@if [ ! -d $(LIB_DIR) ]; then \
		mkdir -p $(LIB_DIR); \
	fi

check_nasm_exists:
	@if ! command -v nasm >/dev/null 2>&1; then \
		unameOut=$$(uname); \
		if [ "$$unameOut" = "Darwin" ]; then \
			echo "Installing nasm via brew..."; \
			brew install nasm; \
		elif [ -f /etc/debian_version ]; then \
			echo "Installing nasm via apt-get..."; \
			sudo apt-get update && sudo apt-get install -y nasm; \
		else \
			echo "Please install nasm manually for your OS."; \
			exit 1; \
		fi \
	fi

check_ffmpeg_exists: check_nasm_exists
	@if [ ! -d $(FFMPEG_DIR) ]; then \
		git clone https://git.ffmpeg.org/ffmpeg.git $(FFMPEG_DIR) && \
		cd $(FFMPEG_DIR) && \
		./configure --disable-shared --enable-static --prefix="$(PWD)/$(FFMPEG_DIR)/build" && \
		make && \
		make install; \
	fi

check_jpeg_exists:
	@if [ ! -d $(JPEG_DIR) ]; then \
		cd $(LIB_DIR) && \
		curl -LO https://ijg.org/files/jpegsrc.v9f.tar.gz && \
		tar xzf jpegsrc.v9f.tar.gz && \
		mv jpeg-9f jpeg && \
		cd jpeg && \
		./configure --disable-shared --enable-static --prefix="$(PWD)/$(JPEG_DIR)" && \
		make && \
		make install; \
		rm -f ../jpegsrc.v9f.tar.gz; \
	fi

check_zlib_exists:
	@if [ ! -d $(ZLIB_DIR) ]; then \
		cd $(LIB_DIR) && \
		curl -LO https://zlib.net/zlib-1.3.1.tar.gz && \
		tar xzf zlib-1.3.1.tar.gz && \
		mv zlib-1.3.1 zlib && \
		cd zlib && \
		./configure --static --prefix="$(PWD)/$(ZLIB_DIR)" && \
		make && \
		make install; \
		rm -f ../zlib-1.3.1.tar.gz; \
	fi

check_png_exists: check_zlib_exists
	@if [ ! -d $(PNG_DIR) ]; then \
		cd $(LIB_DIR) && \
		curl -LO https://download.sourceforge.net/libpng/libpng-1.6.49.tar.gz && \
		tar xzf libpng-1.6.49.tar.gz && \
		mv libpng-1.6.49 png && \
		cd png && \
		./configure --disable-shared --enable-static --with-zlib-prefix="$(PWD)/$(ZLIB_DIR)" --prefix="$(PWD)/$(PNG_DIR)" && \
		make && \
		make install; \
		rm -f ../libpng-1.6.49.tar.gz; \
	fi

check_build_exists:
	@if [ ! -d $(BUILD_DIR) ]; then \
		mkdir -p $(BUILD_DIR); \
	fi

