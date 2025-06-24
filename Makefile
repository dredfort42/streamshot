######################################################################
#                                                                    #
#        ::          ::        +--------+-----------------------+    #
#          ::      ::          | Author | Dmitry Novikov        |    #
#        ::::::::::::::        | Email  | dredfort.42@gmail.com |    #
#      ::::  ::::::  ::::      +--------+-----------------------+    #
#    ::::::::::::::::::::::                                          #
#    ::  ::::::::::::::  ::    File     | Makefile                   #
#    ::  ::          ::  ::    Created  | 2025-06-05                 #
#          ::::  ::::          Modified | 2025-06-23                 #
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

ZLIB_V      := zlib-1.3.1
LIBPNG_V    := libpng-1.6.49
LIBJPEG_V   := jpegsrc.v9f
FFMPEG_V    := ffmpeg-7.1.1

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
BUILD_DIR   := $(CRNT_DIR)/build
TESTS_DIR   := $(CRNT_DIR)/tests

# Platform-specific directories
ifeq ($(PLATFORM),$(MACOS))
	OS_BUILD_DIR := $(BUILD_DIR)/macos
	OS_LIB_DIR   := $(LIB_DIR)/macos
else ifeq ($(PLATFORM),$(LINUX))
	OS_BUILD_DIR := $(BUILD_DIR)/linux
	OS_LIB_DIR   := $(LIB_DIR)/linux
endif

SRCS        := $(shell find $(SRC_DIR) -type f -name '*.c')
OBJS        := $(patsubst $(SRC_DIR)/%.c,$(OS_BUILD_DIR)/%.o,$(SRCS))

# Library directories for Linux static linking
ZLIB_DIR    := $(OS_LIB_DIR)/zlib
PNG_DIR     := $(OS_LIB_DIR)/png
JPEG_DIR    := $(OS_LIB_DIR)/jpeg
FFMPEG_DIR  := $(OS_LIB_DIR)/ffmpeg

# Compiler and flags
INCLUDES    := -I$(HDR_DIR) \
               -I$(ZLIB_DIR)/include \
               -I$(PNG_DIR)/include \
               -I$(JPEG_DIR)/include

LDFLAGS     := -L$(ZLIB_DIR)/lib \
               -L$(PNG_DIR)/lib \
               -L$(JPEG_DIR)/lib

# Platform-specific libraries
ifeq ($(PLATFORM),$(MACOS))
	INCLUDES    += -I/usr/local/opt/ffmpeg/include
	LDFLAGS     += -L/usr/local/opt/ffmpeg/lib
else ifeq ($(PLATFORM),$(LINUX))
	INCLUDES    += -I$(FFMPEG_DIR)/include
	LDFLAGS     += -L$(FFMPEG_DIR)/lib
endif

LIBS        := -lpng \
               -ljpeg \
               -lavformat -lavcodec -lavutil -lswscale \
               -lm -pthread -lz

CFLAGS      := -std=c11 -O2 -DNDEBUG \
               -Wall -Wextra -Werror \
               -fstack-protector-strong -D_FORTIFY_SOURCE=2 \
               -march=native -flto -funroll-loops \
               -MMD -MP \
               $(INCLUDES)

DEV_CFLAGS  := -std=c11 -Wall -Wextra -Wpedantic -Werror -O0 -g \
               -fsanitize=address,undefined,signed-integer-overflow,pointer-compare,pointer-subtract,alignment \
               -fno-omit-frame-pointer -fstack-protector-strong \
               -MMD -MP \
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
RED         := \033[0;31m
GREEN       := \033[0;32m
YELLOW      := \033[1;33m
BLUE        := \033[0;34m
NC          := \033[0m

# Rules
.PHONY: all build dev clean fclean re test help

all: build

build: build_check $(NAME)
	@echo "$(GREEN)Build complete.$(NC)"

$(NAME): $(OBJS)
	@echo "$(YELLOW)Linking $@...$(NC)"
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) $(LIBS)

$(OS_BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "$(YELLOW)Compiling $<...$(NC)"
	$(CC) $(CFLAGS) -c $< -o $@

# --- Dependency handling: ---
# With -MMD -MP, each .o gets a .d file listing its .c and all included .h dependencies.
# If a .c changes, only its .o is rebuilt. If a .h changes, all .o files that depend on it are rebuilt.
# This is the standard and correct way to ensure minimal rebuilds.
#
# -include includes all .d files if they exist, so header changes are tracked automatically.
-include $(OBJS:.o=.d)

dev: CFLAGS := $(DEV_CFLAGS) 
dev: build_check $(NAME)
	@echo "$(GREEN)Development build complete with sanitizers enabled.$(NC)"

clean:
	@rm -rf $(OS_BUILD_DIR)
	@rm -rf debug_files
	@echo "$(GREEN)Cleaned build artifacts and debug files.$(NC)"

fclean: clean
	@rm -rf $(BUILD_DIR)
	@rm -rf $(LIB_DIR)
	@rm -rf *.dSYM
	@rm -f $(APPLICATION)*
	@echo "$(GREEN)Fully cleaned all artifacts and libraries.$(NC)"

re: clean all

help:
	@echo "$(YELLOW)Available targets:$(NC)"
	@echo "  all     - Build the project (default)"
	@echo "  build   - Build the project"
	@echo "  dev     - Build the project with sanitizers enabled for development"
	@echo "  clean   - Remove build artifacts"
	@echo "  fclean  - Remove all build artifacts and libraries"
	@echo "  re      - Clean and rebuild"
	@echo "  test    - Build and run tests"
	@echo "  help    - Show this help message"

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
		echo "$(GREEN)Platform detected: $(PLATFORM)$(NC)"; \
	fi

check_src_exists:
	@if [ ! -d src ]; then \
		echo "$(RED)Error: src directory does not exist.$(NC)"; \
		exit 1; \
	fi

check_lib_exists:
	@if [ ! -d $(OS_LIB_DIR) ]; then \
		mkdir -p $(OS_LIB_DIR); \
	fi

check_build_exists:
	@if [ ! -d $(OS_BUILD_DIR) ]; then \
		mkdir -p $(OS_BUILD_DIR); \
	fi

# Check zlib installation
check_zlib_exists:
	@if [ ! -d $(ZLIB_DIR) ]; then \
		echo "$(YELLOW)Installing zlib (static build for production)...$(NC)"; \
		cd $(OS_LIB_DIR) && \
		if [ ! -d zlib_tmp ]; then \
			echo "$(BLUE)Downloading zlib source code...$(NC)"; \
			curl -LO https://zlib.net/$(ZLIB_V).tar.gz && \
			tar xzf $(ZLIB_V).tar.gz && \
			mv $(ZLIB_V) zlib_tmp && \
			rm -f $(ZLIB_V).tar.gz; \
		fi; \
		cd zlib_tmp && \
		echo "$(BLUE)Configuring zlib for static build...$(NC)" && \
		./configure --static --prefix="$(ZLIB_DIR)" && \
		echo "$(BLUE)Building zlib...$(NC)" && \
		make -j$(N_CPU) && \
		if [ $$? -ne 0 ]; then \
			echo "$(RED)Error: Failed to make zlib.$(NC)"; \
			exit 1; \
		fi; \
		echo "$(BLUE)Installing zlib static library...$(NC)" && \
		make install && \
		if [ $$? -ne 0 ]; then \
			echo "$(RED)Error: Failed to install zlib.$(NC)"; \
			rm -rf $(ZLIB_DIR); \
			exit 1; \
		fi; \
		cd .. && \
		rm -rf zlib_tmp; \
		echo "$(GREEN)zlib static library installed successfully.$(NC)"; \
	else \
		echo "$(GREEN)zlib exists.$(NC)"; \
	fi

# Check libpng installation
check_png_exists: check_zlib_exists
	@if [ ! -d $(PNG_DIR) ]; then \
		echo "$(YELLOW)Installing libpng (static build for production)...$(NC)"; \
		cd $(OS_LIB_DIR) && \
		if [ ! -d png_tmp ]; then \
			echo "$(BLUE)Downloading libpng source code...$(NC)" && \
			curl -LO https://download.sourceforge.net/libpng/$(LIBPNG_V).tar.gz && \
			tar xzf $(LIBPNG_V).tar.gz && \
			mv $(LIBPNG_V) png_tmp && \
			rm -f $(LIBPNG_V).tar.gz; \
		fi; \
		cd png_tmp && \
		echo "$(BLUE)Configuring libpng for static build...$(NC)" && \
		./configure --disable-shared --enable-static --with-zlib-prefix="$(ZLIB_DIR)" --prefix="$(PNG_DIR)" && \
		echo "$(BLUE)Building libpng...$(NC)" && \
		make -j$(N_CPU) && \
		if [ $$? -ne 0 ]; then \
			echo "$(RED)Error: Failed to make libpng.$(NC)"; \
			exit 1; \
		fi; \
		echo "$(BLUE)Installing libpng static library...$(NC)" && \
		make install; \
		if [ $$? -ne 0 ]; then \
			echo "$(RED)Error: Failed to install libpng.$(NC)"; \
			rm -rf $(PNG_DIR); \
			exit 1; \
		fi; \
		cd .. && \
		rm -rf png_tmp; \
		echo "$(GREEN)libpng static library installed successfully.$(NC)"; \
	else \
		echo "$(GREEN)libpng exists.$(NC)"; \
	fi

# Check libjpeg installation
check_jpeg_exists:
	@if [ ! -d $(JPEG_DIR) ]; then \
		echo "$(YELLOW)Installing libjpeg (static build for production)...$(NC)"; \
		cd $(OS_LIB_DIR) && \
		if [ ! -d jpeg_tmp ]; then \
			echo "$(BLUE)Downloading ffmpeg source code for libjpeg...$(NC)"; \
			curl -LO  https://ijg.org/files/$(LIBJPEG_V).tar.gz && \
			tar xzf $(LIBJPEG_V).tar.gz && \
			mv jpeg-9f jpeg_tmp && \
			rm -f $(LIBJPEG_V).tar.gz; \
		fi; \
		cd jpeg_tmp; \
		echo "$(BLUE)Configuring libjpeg for static build...$(NC)" && \
		./configure --disable-shared --enable-static --prefix="$(JPEG_DIR)" && \
		echo "$(BLUE)Building libjpeg...$(NC)" && \
		make -j$(N_CPU) && \
		if [ $$? -ne 0 ]; then \
			echo "$(RED)Error: Failed to make libjpeg.$(NC)"; \
			exit 1; \
		fi; \
		echo "$(BLUE)Installing libjpeg static library...$(NC)" && \
		make install; \
		if [ $$? -ne 0 ]; then \
			echo "$(RED)Error: Failed to install libjpeg.$(NC)"; \
			rm -rf $(JPEG_DIR); \
			exit 1; \
		fi; \
		cd .. && \
		rm -rf jpeg_tmp; \
		echo "$(GREEN)libjpeg static library installed successfully.$(NC)"; \
	else \
		echo "$(GREEN)libjpeg exists.$(NC)"; \
	fi

# Check ffmpeg installation
check_ffmpeg_exists:
	@if [ "$(PLATFORM)" = "$(MACOS)" ] && ! command -v ffmpeg >/dev/null 2>&1; then \
		echo "$(YELLOW)ffmpeg not found. Installing...$(NC)"; \
		$(PKG_INSTALL) ffmpeg; \
	elif [ "$(PLATFORM)" = "$(LINUX)" ] && [ ! -d $(FFMPEG_DIR) ]; then \
		echo "$(YELLOW)ffmpeg not found. Installing...$(NC)"; \
		if ! command -v nasm >/dev/null 2>&1; then \
			echo "$(YELLOW)nasm not found. Installing...$(NC)"; \
			$(PKG_INSTALL) nasm; \
		fi; \
		cd $(OS_LIB_DIR); \
		if [ ! -d ffmpeg_tmp ]; then \
			echo "$(BLUE)Downloading ffmpeg source code...$(NC)"; \
			curl -LO https://ffmpeg.org/releases/$(FFMPEG_V).tar.gz && \
			tar xzf $(FFMPEG_V).tar.gz && \
			mv $(FFMPEG_V) ffmpeg_tmp && \
			rm -f $(FFMPEG_V).tar.gz; \
		fi; \
		cd ffmpeg_tmp; \
		echo "$(BLUE)Configuring ffmpeg for static build...$(NC)"; \
		./configure \
			--prefix=$(FFMPEG_DIR) \
			--extra-cflags="-fPIC" \
			--extra-ldflags="-static" \
			--enable-gpl \
			--enable-version3 \
			--enable-small \
			--disable-swscale-alpha \
			--disable-avdevice \
			--disable-swresample \
			--disable-postproc \
			--disable-shared \
			--disable-doc \
			--disable-programs; \
		echo "$(BLUE)Building ffmpeg...$(NC)"; \
		make -j$(N_CPU) && \
		if [ $$? -ne 0 ]; then \
			echo "$(RED)Error: Failed to make ffmpeg.$(NC)"; \
			exit 1; \
		fi; \
		echo "$(BLUE)Installing ffmpeg static library...$(NC)"; \
		make install; \
		if [ $$? -ne 0 ]; then \
			echo "$(RED)Error: Failed to install ffmpeg.$(NC)"; \
			rm -rf $(FFMPEG_DIR); \
			exit 1; \
		fi; \
		cd ..; \
		rm -rf ffmpeg_tmp; \
		echo "$(GREEN)ffmpeg static library installed successfully.$(NC)"; \
	else \
		echo "$(GREEN)ffmpeg exists.$(NC)"; \
	fi
