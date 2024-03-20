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
	-pedantic  
# -O3 \
# -D XRE_DISABLE_CHECKS=1 \
# -D XRE_DISABLE_ASSERTS=1

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
	runtime-v2/xre_runtime.c \
	runtime-v2/tree.c \
\
	runtime-v2/operations/relational/eq.c \
	runtime-v2/operations/relational/gt.c \
	runtime-v2/operations/relational/le.c \
	runtime-v2/operations/relational/ge.c \
	runtime-v2/operations/relational/lt.c \
	runtime-v2/operations/relational/relational.c \
	runtime-v2/operations/relational/ne.c \
	runtime-v2/operations/relational/not.c \
\
	runtime-v2/operations/value/value.c \
\
	runtime-v2/operations/assignment/mod_assign.c \
	runtime-v2/operations/assignment/mul_assign.c \
	runtime-v2/operations/assignment/assign.c \
	runtime-v2/operations/assignment/assignment.c \
	runtime-v2/operations/assignment/sub_assign.c \
	runtime-v2/operations/assignment/div_assign.c \
	runtime-v2/operations/assignment/add_assign.c \
	runtime-v2/operations/assignment/pow_assign.c \
\
	runtime-v2/operations/logical/or.c \
	runtime-v2/operations/logical/loop.c \
	runtime-v2/operations/logical/logical.c \
	runtime-v2/operations/logical/and.c \
	runtime-v2/operations/logical/do.c \
	runtime-v2/operations/logical/else.c \
\
	runtime-v2/operations/arithmetic/mul.c \
	runtime-v2/operations/arithmetic/rshift.c \
	runtime-v2/operations/arithmetic/mod.c \
	runtime-v2/operations/arithmetic/lshift.c \
	runtime-v2/operations/arithmetic/arithmetic.c \
	runtime-v2/operations/arithmetic/div.c \
	runtime-v2/operations/arithmetic/bw_xor.c \
	runtime-v2/operations/arithmetic/pow.c \
	runtime-v2/operations/arithmetic/bw_and.c \
	runtime-v2/operations/arithmetic/add.c \
	runtime-v2/operations/arithmetic/sub.c \
	runtime-v2/operations/arithmetic/bw_or.c \

	
