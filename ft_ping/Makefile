NAME := ft_ping
CC := cc
CFLAGS := -Wall -Wextra -Werror
CPPFLAGS := -Iincludes

SRC := src/main.c \
       src/parse.c \
       src/resolve.c \
       src/packet.c \
       src/loop.c \
       src/reply.c \
       src/stats.c
OBJ := $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -lm -o $@

%.o: %.c includes/ft_ping.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
