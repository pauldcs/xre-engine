NAME           := xre
CC             := clang
SRCS_DIR       := srcs
OBJS_DIR       := .objs
INCS_DIR       := incs
LIBARRAY_DIR   := srcs/core/array
LIBARRAY_NAME  := libarray.a

#MAKEFLAGS += -j6

CFLAGS_RELEASE := \
	-O3                     \
	-D XRE_DISABLE_CHECKS=1 \
	-D XRE_DISABLE_ASSERTS=1

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
	main.c \
	core/args/xre_args.c \
	core/fs/xre_fs.c \
	core/log/xre_log.c \
	core/ds/dynstr.c \
\
	core/utils/stringf/xre_stringf.c \
	core/utils/hex/xre_xdp.c \
	core/utils/str_escape.c \
	core/utils/str_unescape.c \
	core/utils/str_to_values.c \
	core/utils/str_to_int32.c \
	core/utils/xalloc.c \
	core/utils/shortname.c \
	core/utils/st_mode_to_string.c \
	core/utils/timespec_to_string.c \
	core/utils/stresc2chr.c \
	core/utils/hash_string.c \
	core/utils/str_to_int32.c \
	core/error/xre_errors.c \
	core/error/xre_print_error.c \
	core/utils/xwrite.c \
\
	parse/lex.c \
	parse/syntax.c \
	parse/parse.c \
	parse/utils.c \
	parse/compose.c \
	parse/precedence.c \
\
	repl/xre_repl.c \
	repl/xre_repl_read.c \
	repl/xre_repl_signals.c \
\
	repl/readline/xre_readline.c \
	repl/readline/rl_terminal.c \
	repl/readline/rl_globals.c \
	repl/readline/rl_utils.c \
	repl/readline/rl_actions.c \
	repl/readline/rl_display.c \
	repl/readline/rl_keys.c \
	repl/readline/rl_line_mode.c \
	repl/readline/rl_editor_mode.c \
	repl/readline/rl_buffer.c \
\
	runtime/xre_runtime.c \
	runtime/common.c \
\
	runtime/operations/scope_resolution.c \
	runtime/operations/eq.c \
	runtime/operations/gt.c \
	runtime/operations/le.c \
	runtime/operations/ge.c \
	runtime/operations/lt.c \
	runtime/operations/ne.c \
	runtime/operations/not.c \
	runtime/operations/value.c \
	runtime/operations/string.c \
	runtime/operations/assign.c \
	runtime/operations/or.c \
	runtime/operations/loop.c \
	runtime/operations/and.c \
	runtime/operations/do.c \
	runtime/operations/else.c \
	runtime/operations/separator.c \
	runtime/operations/sequence.c \
	runtime/operations/symbol.c \
	runtime/operations/mul.c \
	runtime/operations/rshift.c \
	runtime/operations/mod.c \
	runtime/operations/lshift.c \
	runtime/operations/div.c \
	runtime/operations/bw_xor.c \
	runtime/operations/pow.c \
	runtime/operations/bw_and.c \
	runtime/operations/add.c \
	runtime/operations/sub.c \
	runtime/operations/bw_or.c \
	runtime/operations/symbol.c \
\
	runtime/memory/heap.c \
	runtime/memory/stack.c \
	runtime/memory/symtab.c \
\
	runtime/memory/object/register.c \
	runtime/memory/object/sequence.c \
	runtime/memory/object/string.c \
	runtime/memory/object/symbol.c \
	runtime/memory/object/undefined.c \
\
	runtime/builtin/builtins.c \
	runtime/builtin/xre_builtin.c \
\
	runtime/builtin/std/array/array_size.c \
	runtime/builtin/std/str/str_size.c \
\
	runtime/builtin/std/os/print.c \
	runtime/builtin/std/os/yes.c \
	runtime/builtin/std/os/typeof.c
	