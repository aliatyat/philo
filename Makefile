NAME = philo

CC = cc
CFLAGS = -Wall -Werror -Wextra 


SRCS = philo.c

OBJS = $(SRCS:.c=.o)

all :	$(NAME)

$(NAME): philo.h $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

clean :
	rm -f $(OBJS)

fclean : clean
	rm -f $(NAME)

re : fclean all

.PHONY: fclean clean all re