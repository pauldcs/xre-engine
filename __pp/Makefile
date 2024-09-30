NAME       := bin
CC         := clang
SRCS_DIR   := .
OBJS_DIR   := .objs
INCS_DIR   := .
CFLAGS     := \
	-Wall \
	-Wextra \
	-Werror \
	-fstack-protector \
	-fstack-protector-strong \
	-fstack-check 

SRCS := \
	main.c 

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
	$(CC) \
		$^ \
		$(CFLAGS) \
		-o $(NAME) \
		-I $(INCS_DIR) \
		-ltermcap 

p: CFLAGS += $(CLFAGS_NOSEC)
p: all

sec: CFLAGS += $(CLFAGS_SEC)
sec: all

clean:
	rm -rf $(OBJS_DIR)
	rm -rf .cache
	rm -rf .dSYM

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY	: all clean p fclean re 
