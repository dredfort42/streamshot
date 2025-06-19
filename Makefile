######################################################################
#                                                                    #
#        ::          ::        +--------+-----------------------+    #
#          ::      ::          | Author | Dmitry Novikov        |    #
#        ::::::::::::::        | Email  | dredfort.42@gmail.com |    #
#      ::::  ::::::  ::::      +--------+-----------------------+    #
#    ::::::::::::::::::::::                                          #
#    ::  ::::::::::::::  ::    File     | Makefile                   #
#    ::  ::          ::  ::    Created  | 2025-06-05                 #
#          ::::  ::::          Modified | 2025-06-06                 #
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
FFMPEG_LIBS := avformat avutil avcodec swscale

CC          := gcc
OBJS        := $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SRCS))

HDRS        := include
INCLUDES    := -I$(HDRS) -I$(FFMPEG_DIR)/libavformat -I$(FFMPEG_DIR)/libavutil -I$(FFMPEG_DIR)/libavcodec -I$(FFMPEG_DIR)/libswscale
LDFLAGS     := -L$(FFMPEG_DIR) -lavformat -lavutil -lavcodec -lswscale

# -O3: Enables high-level optimizations for speed. The compiler tries to make your code run as fast as possible.
# -march=native: Generates code optimized for the architecture of the machine compiling the code. It uses all instruction sets available on your CPU.
# -flto: Enables Link Time Optimization, allowing the compiler to optimize across files during the linking stage for better performance.
# -funroll-loops: Tells the compiler to unroll loops where possible, which can make loops run faster at the cost of larger binaries.
CFLAGS      := -O3 -march=native -flto -funroll-loops -Wall -Wextra -Werror $(INCLUDES)
# CFLAGS      := -Wall -Wextra -Werror $(INCLUDES)
# CFLAGS      := -g -O0 -fsanitize=address -Wall -Wextra -Werror $(INCLUDES)

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
build_check: check_src_exists check_build_exists check_lib_exists check_ffmpeg_exists
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

check_ffmpeg_exists:
	@if [ ! -d $(FFMPEG_DIR) ]; then \
		git clone https://git.ffmpeg.org/ffmpeg.git $(FFMPEG_DIR); \
	fi

check_build_exists:
	@if [ ! -d $(BUILD_DIR) ]; then \
		mkdir -p $(BUILD_DIR); \
	fi

