NAME          := libarray.a
CC            := gcc
SRCS_DIR      := srcs
OBJS_DIR      := .objs
BUILD_DIR     := build
INCS_DIR      := srcs

TEST_FRAMEWORK_BIN := tester
TEST_FRAMEWORK_DIR := unit-tests
TEST_FRAMEWORK_INCS_DIR := $(TEST_FRAMEWORK_DIR)/srcs
TEST_FRAMEWORK_SRCS := \
	$(TEST_FRAMEWORK_DIR)/main.c \
	$(TEST_FRAMEWORK_DIR)/srcs/unit_tests.c \
	$(TEST_FRAMEWORK_DIR)/srcs/asserts.c

CFLAGS := \
	-Wall     \
	-Wextra   \
	-Werror   \
	-pedantic 

# export ASAN_OPTIONS="log_path=sanitizer.log"
# export ASAN_OPTIONS="detect_leaks=1"

CFLAGS_DBG := \
	-g3                      \
	-O0                      
# -fsanitize=address       \
# -fsanitize=undefined     \
# -fno-omit-frame-pointer  \
# -fstack-protector-strong \
# -fno-optimize-sibling-calls 

SRCS := \
	array.c 
