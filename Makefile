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

APPLICATION := streamshot
VERSION     := 1.0.0

MACOS       := macos
LINUX       := linux
UNSUPPORTED := unsupported

# Platform detection
UNAME_S     := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	PLATFORM    := $(MACOS)
	PKG_INSTALL := brew install
	N_CPU       := $(shell sysctl -n hw.ncpu)
	NAME        := $(APPLICATION)_$(MACOS)_v$(VERSION)
else ifeq ($(UNAME_S),Linux)
	PLATFORM    := $(LINUX)
	PKG_INSTALL := sudo apt-get install -y
	N_CPU       := $(shell nproc)
	NAME        := $(APPLICATION)_$(LINUX)_v$(VERSION)
else
	PLATFORM    := $(UNSUPPORTED)
endif

CRNT_DIR    := $(shell pwd)

# Variables
HDR_DIR     := $(CRNT_DIR)/include
SRC_DIR     := $(CRNT_DIR)/src
LIB_DIR     := $(CRNT_DIR)/lib
TESTS_DIR   := $(CRNT_DIR)/tests

# Platform-specific directories
ifeq ($(PLATFORM),$(MACOS))
	BUILD_DIR := $(CRNT_DIR)/build/macos
else ifeq ($(PLATFORM),$(LINUX))
	BUILD_DIR := $(CRNT_DIR)/build/linux
endif

SRCS        := $(shell find $(SRC_DIR) -type f -name '*.c')
OBJS        := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# Library directories for Linux static linking
ZLIB_DIR    := $(LIB_DIR)/zlib
PNG_DIR     := $(LIB_DIR)/png
JPEG_DIR    := $(LIB_DIR)/jpeg
FFMPEG_DIR  := $(LIB_DIR)/ffmpeg

# Compiler and flags
INCLUDES	:= -I$(HDR_DIR) \
			   -I$(ZLIB_DIR)/include \
			   -I$(PNG_DIR)/include \
			   -I$(JPEG_DIR)/include

LDFLAGS		:= -L$(ZLIB_DIR)/lib \
			   -L$(PNG_DIR)/lib \
			   -L$(JPEG_DIR)/lib

LIBS		:= -lpng -ljpeg -lz -lm -pthread

CFLAGS      := -std=c11 -O2 -DNDEBUG \
               -Wall -Wextra -Werror \
               -fstack-protector-strong -D_FORTIFY_SOURCE=2 \
               -march=native -flto -funroll-loops \
               $(INCLUDES)

DEV_CFLAGS  := -std=c11 -Wall -Wextra -Wpedantic -Werror -O0 -g \
               -fsanitize=address,undefined,signed-integer-overflow,pointer-compare,pointer-subtract,alignment \
               -fno-omit-frame-pointer -fstack-protector-strong \
               $(INCLUDES)

# Platform-specific flags
ifeq ($(PLATFORM),$(MACOS))
	CFLAGS      += -Wno-error=visibility
	DEV_CFLAGS  += -Wno-error=visibility
else ifeq ($(PLATFORM),$(LINUX))
	CFLAGS      += -Wno-error=unused-result
	DEV_CFLAGS  += -Wno-error=unused-result
endif

CC          := gcc

# Output colors
GREEN       := \033[0;32m
RED         := \033[0;31m
YELLOW      := \033[1;33m
NC          := \033[0m

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
	@rm -rf $(LIB_DIR)
	@rm -rf *.dSYM
	@rm -f $(APPLICATION)*
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
build_check: check_platform \
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

# Check zlib installation
check_zlib_exists:
	@if [ ! -d $(ZLIB_DIR) ]; then \
		echo "$(YELLOW)Installing zlib (static build for production)...$(NC)"; \
		cd $(LIB_DIR) && \
		curl -LO https://zlib.net/zlib-1.3.1.tar.gz && \
		tar xzf zlib-1.3.1.tar.gz && \
		mv zlib-1.3.1 zlib_tmp && \
		cd zlib_tmp && \
		./configure --static --prefix="$(ZLIB_DIR)" && \
		make clean && \
		make -j2 && \
		make install && \
		cd .. && \
		rm -rf zlib_tmp zlib-1.3.1.tar.gz; \
		echo "$(GREEN)zlib static library installed successfully.$(NC)"; \
	else \
		echo "$(GREEN)zlib exists.$(NC)"; \
	fi

# Check libpng installation
check_png_exists: check_zlib_exists
	@if [ ! -d $(PNG_DIR) ]; then \
		echo "$(YELLOW)Installing libpng (static build for production)...$(NC)"; \
		cd $(LIB_DIR) && \
		curl -LO https://download.sourceforge.net/libpng/libpng-1.6.49.tar.gz && \
		tar xzf libpng-1.6.49.tar.gz && \
		mv libpng-1.6.49 png_tmp && \
		cd png_tmp && \
		./configure --disable-shared --enable-static --with-zlib-prefix="$(ZLIB_DIR)" --prefix="$(PNG_DIR)" && \
		make && \
		make install; \
		cd .. && \
		rm -rf png_tmp libpng-1.6.49.tar.gz; \
		echo "$(GREEN)libpng static library installed successfully.$(NC)"; \
	else \
		echo "$(GREEN)libpng exists.$(NC)"; \
	fi

# Check libjpeg installation
check_jpeg_exists:
	@if [ ! -d $(JPEG_DIR) ]; then \
		echo "$(YELLOW)Installing libjpeg (static build for production)...$(NC)"; \
		cd $(LIB_DIR) && \
		curl -LO https://ijg.org/files/jpegsrc.v9f.tar.gz && \
		tar xzf jpegsrc.v9f.tar.gz && \
		mv jpeg-9f jpeg_tmp && \
		cd jpeg_tmp && \
		./configure --disable-shared --enable-static --prefix="$(JPEG_DIR)" && \
		make && \
		make install; \
		cd .. && \
		rm -rf jpeg_tmp jpegsrc.v9f.tar.gz; \
		echo "$(GREEN)libjpeg static library installed successfully.$(NC)"; \
	else \
		echo "$(GREEN)libjpeg exists.$(NC)"; \
	fi

# Check ffmpeg installation
check_ffmpeg_exists:
	@echo "$(YELLOW)Checking for ffmpeg...$(NC)"; \
	if ! command -v ffmpeg >/dev/null 2>&1; then \
		if [ "$(PLATFORM)" = "$(MACOS)" ]; then \
			echo "$(YELLOW)ffmpeg not found. Installing...$(NC)"; \
			$(PKG_INSTALL) ffmpeg; \
		elif [ "$(PLATFORM)" = "$(LINUX)" ]; then \
			echo "$(YELLOW)ffmpeg not found. Installing...$(NC)"; \
			$(PKG_INSTALL) ffmpeg libavformat-dev libavcodec-dev libavutil-dev libswscale-dev; \
		else \
			echo "$(RED)Please install ffmpeg manually for your OS.$(NC)"; \
			exit 1; \
		fi; \
	else \
		echo "$(GREEN)ffmpeg found.$(NC)"; \
	fi

ifeq ($(PLATFORM),$(MACOS))
INCLUDES += -I/usr/local/opt/ffmpeg/include
LDFLAGS  += -L/usr/local/opt/ffmpeg/lib
LIBS     += -lavformat -lavcodec -lavutil -lswscale
endif
