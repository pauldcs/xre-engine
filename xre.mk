NAME          := xre
CC            := clang
SRCS_DIR      := srcs
OBJS_DIR      := .objs
INCS_DIR      := incs

LIBARRAY_DIR   := srcs/core/array
LIBARRAY_NAME  := libarray.a

CFLAGS   := \
	-Wall     \
	-Wextra   \
	-Werror   \
	-pedantic \
	-g3 \
#-O3 \
#-D XRE_DISABLE_CHECKS=1 \
#-D XRE_DISABLE_ASSERTS=1

# export ASAN_OPTIONS="log_path=sanitizer.log"
# export ASAN_OPTIONS="detect_leaks=1"

CFLAGS_DBG              := \
	-O0                      \
	-fsanitize=address       \
	-fsanitize=undefined     \
	-fno-omit-frame-pointer  \
	-fstack-protector-strong \
	-fno-optimize-sibling-calls 

SRCS := \
	main.c \
	core/args/xre_args.c \
	core/fs/xre_fs.c \
	core/utils/xwrite.c \
	core/hex/xre_xdp.c \
	core/log/xre_log.c \
	core/stringf/xre_stringf.c \
\
	core/utils/str_to_values.c \
	core/utils/str_to_int32.c \
	core/utils/xalloc.c \
	core/utils/shortname.c \
	core/utils/st_mode_to_string.c \
	core/utils/timespec_to_string.c \
	core/utils/stresc2chr.c \
	core/utils/str2print.c \
	core/utils/hash_string.c \
	core/utils/str_to_int32.c \
\
	parse/lex.c \
	parse/syntax.c \
	parse/parse.c \
	parse/utils.c \
	parse/compose.c \
\
	error/xre_errors.c \
	error/xre_print_error.c \
\
	repl/xre_repl.c \
	repl/xre_repl_read.c \
	repl/xre_repl_signals.c \
\
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
	runtime/frame_tree.c \
	runtime/symbols.c \
	runtime/state.c \
\
	runtime/operations/operators/arithmetic.c \
	runtime/operations/operators/assignment.c \
	runtime/operations/operators/logical.c \
	runtime/operations/operators/bitwise.c \
	runtime/operations/operators/loop.c \
	runtime/operations/operators/not.c \
	runtime/operations/operators/relational.c \
	runtime/operations/operators/sequence.c \
	runtime/operations/operators/at.c \
	runtime/operations/operators/print.c \
	runtime/operations/operands/identifier.c \
	runtime/operations/operands/basic.c 
	
