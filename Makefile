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
	$(MAKE) -C $(LIBCONT_DIR)
	$(CC) \
		$^ \
		$(CFLAGS) \
		$(MAIN) \
		-o $(NAME) \
		-I $(INCS_DIR) \
		-L $(LIBCONT_DIR) \
		-lcont \
		-ltermcap 

g: CFLAGS += $(CFLAGS_DBG)
g: all

clean:
	$(MAKE) clean -C $(LIBCONT_DIR)
	rm -rf *.dSYM
	rm -rf .cache
	rm -rf .vscode
	rm -rf $(OBJS_DIR)

fclean: clean
	$(MAKE) fclean -C $(LIBCONT_DIR)
	rm -rf $(NAME)

re: fclean all

.PHONY	: all clean g fclean re 
