include xre.mk

SRCS_OBJS := $(patsubst %.c,$(OBJS_DIR)/%.o,$(SRCS))

$(OBJS_DIR)/%.o:$(SRCS_DIR)/%.c
	mkdir -vp $(dir $@)
#	@printf "compiling: %s\n" $<
	$(CC) \
		$(CFLAGS) \
		-MMD \
		-MP \
		-o $@ \
		-c $< \
		-I $(INCS_DIR)

all: $(NAME)

-include  $(SRCS_OBJS:.o=.d)

$(NAME): $(SRCS_OBJS)
	$(MAKE) -C $(LIBARRAY_DIR)
	$(CC) \
		$^ \
		$(CFLAGS) \
		-o $(NAME) \
		-I $(INCS_DIR) \
		-L $(LIBARRAY_DIR) \
		-larray \
		-ltermcap 

asan: CFLAGS += $(CFLAGS_ASAN)
asan: all
	$(MAKE) fclean -C $(LIBARRAY_DIR)
	$(MAKE) asan -C $(LIBARRAY_DIR)

release: CFLAGS = $(CFLAGS_RELEASE)
release: all

format:
	find . \( -name "*.c" -o -name "*.h" \) \
		-type f \
		-exec \
			clang-format \
			--verbose \
			-style=file -i {} \;

sparse:
	find . -name "*.c" \
		-not -path "*/unit-tests/*" \
		-type f \
		-exec \
			sparse           \
				-Wno-decl      \
				-Wsparse-error \
				-I $(INCS_DIR)  \
				-style=file -i {} \; 

clean:
	$(MAKE) clean -C $(LIBARRAY_DIR)
	rm -rf *.dSYM
	rm -rf .cache
	rm -rf .vscode
	rm -rf $(OBJS_DIR)

fclean: clean
	$(MAKE) fclean -C $(LIBARRAY_DIR)
	rm -f $(NAME)

re: fclean all
ra: fclean asan

.PHONY	: \
	all     \
	clean   \
	release \
	sparse  \
	fclean  \
	format  \
	re      \
	ra
