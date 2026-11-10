NAME := ft_traceroute
CC := cc
CFLAGS := -Wall -Wextra -Werror
CPPFLAGS := -Iincludes

SRC := src/main.c \
       src/parse.c \
       src/resolve.c \
       src/packet.c \
       src/trace.c
OBJ := $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

%.o: %.c includes/ft_traceroute.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
