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

CFLAGS_RELEASE := \
	-O3                     \
	-D DISABLE_HARDENED_RUNTIME=1

CFLAGS := \
	-O0     \
	-g3     \
	-Wall   \
	-Wextra \
	-Werror \
	-pedantic

CFLAGS_ASAN := \
	-fsanitize=address        \
	-fsanitize=undefined      \
	-fno-omit-frame-pointer   \
	-fstack-protector-strong  \
	-fno-optimize-sibling-calls 

SRCS := \
	array.c 
