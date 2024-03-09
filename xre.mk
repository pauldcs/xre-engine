NAME          := xre
CROSS_COMPILE := 
CC            := $(CROSS_COMPILE)gcc
SRCS_DIR      := srcs
OBJS_DIR      := .objs
BUILD_DIR     := build
INCS_DIR      := incs
TEST_MAIN     := srcs/tests/__main__.c
LIBCONT_DIR   := srcs/core/array
LIBCONT_NAME  := libcont.dylib
MAIN          := srcs/__main__.c

CFLAGS   := \
	-Wall     \
	-Wextra   \
	-pedantic \
	-O3 \
	-g3

# export ASAN_OPTIONS="log_path=sanitizer.log"
# export ASAN_OPTIONS="detect_leaks=1"

CFLAGS_DBG              := \
	-g3                      \
	-O0                      \
	-fsanitize=address       \
	-fsanitize=undefined     \
	-fno-omit-frame-pointer  \
	-fstack-protector-strong \
	-fno-optimize-sibling-calls 

SRCS := \
	core/args/xre_args.c \
	core/fs/xre_fs.c \
	core/utils/str_to_values.c \
	core/utils/str_to_int32.c \
	core/utils/xwrite.c \
	core/hex/xre_xdp.c \
	core/utils/xalloc.c \
	core/utils/shortname.c \
	core/utils/st_mode_to_string.c \
	core/utils/timespec_to_string.c \
	core/utils/stresc2chr.c \
	core/utils/str2print.c \
	core/utils/hash_string.c \
	core/utils/str_to_int32.c \
	core/stringf/xre_stringf.c \
	core/log/xre_log.c \
	parse/lex.c \
	parse/syntax.c \
	parse/parse.c \
	parse/utils.c \
	parse/compose.c \
	error/xre_errors.c \
	error/xre_print_error.c \
	repl/xre_repl.c \
	repl/xre_repl_read.c \
	repl/xre_repl_signals.c \
	readline/xre_readline.c \
	readline/rl_terminal.c \
	readline/rl_globals.c \
	readline/rl_utils.c \
	readline/rl_actions.c \
	readline/rl_display.c \
	readline/rl_keys.c \
	readline/rl_line_mode.c \
	readline/rl_editor_mode.c \
	readline/rl_buffer.c \
\
	runtime/xre_runtime.c \
	runtime/state/runtime_stack.c \
	runtime/state/state.c \
	runtime/operations/arithmetic.c \
	runtime/operations/assignment.c \
	runtime/operations/logical.c \
	runtime/operations/bitwise.c \
	runtime/operations/loop.c \
	runtime/operations/not.c \
	runtime/operations/relational.c \
	runtime/operations/sequence.c \
	runtime/operations/inject.c \
\
	runtime/operands/operands.c 
