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
BUILD_DIR   := build
LIB_DIR     := lib

SRCS        := $(wildcard $(SRC_DIR)/*.c)

#FFMPEG library
FFMPEG_DIR 	:= ${LIB_DIR}/ffmpeg
FFMPEG_LIBS := avformat avutil avcodec swscale

CC          := gcc
OBJS        := $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SRCS))

HDRS        := include
INCLUDES    := -I$(HDRS) -I$(FFMPEG_DIR)/libavformat -I$(FFMPEG_DIR)/libavutil -I$(FFMPEG_DIR)/libavcodec -I$(FFMPEG_DIR)/libswscale
CFLAGS      := -Wall -Wextra -Werror $(INCLUDES)
LDFLAGS     := -L$(FFMPEG_DIR) -lavformat -lavutil -lavcodec -lswscale

# Rules
.PHONY: all build clean fclean re

all: build

build: build_check $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(BUILD_DIR)

fclean: clean
	@rm -rf debug_files
	@rm -rf $(LIB_DIR)
	@rm -f $(NAME)

re: clean all

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

