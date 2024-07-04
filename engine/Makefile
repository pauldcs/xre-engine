include xre.mk

SRCS_OBJS := $(patsubst %.c,$(OBJS_DIR)/%.o,$(SRCS))

$(OBJS_DIR)/%.o:$(SRCS_DIR)/%.c
	@mkdir -vp $(dir $@)
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

debug: all

asan: CFLAGS += $(CFLAGS_ASAN)
asan: all

release: CFLAGS = $(CFLAGS_RELEASE)
release: all

format:
	find . \
		-name "*.c" \
		-exec \
			clang-format \
			--verbose \
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

.PHONY	: all clean debug release fclean format re 
