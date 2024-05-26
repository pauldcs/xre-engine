include libarray.mk
include specs.mk

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
	ar rc \
		$(NAME) \
		$(SRCS_OBJS) 

g: CFLAGS += $(CFLAGS_DBG)
g: all

specs: g
	$(CC) \
		$(TEST_FRAMEWORK_SRCS) \
		$(SPECS_SRCS) \
		$(CFLAGS) \
		-I $(INCS_DIR) \
		-I $(TEST_FRAMEWORK_INCS_DIR) \
		-L. $(NAME) \
		-o tester

clean:
	rm -rf *.dSYM
	rm -rf .cache
	rm -rf $(OBJS_DIR)

fclean: clean
	rm -rf $(NAME)
	rm -rf $(TEST_FRAMEWORK_BIN) 

re: fclean all

.PHONY	: all clean g specs fclean re 
