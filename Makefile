FILES	=	Pratt.c singleton.c utils.c
SRC_DIR	=	srcs
SRCS	=	$(addprefix $(SRC_DIR)/, $(FILES))
OBJ_DIR	=	.objs
OBJS	=	$(addprefix $(OBJ_DIR)/, $(FILES:.c=.o))

NAME	=	Pratt
CC		=	gcc
CFLAGS	=	-g3 
DEBUG	=	-fsanitize=address
RM		=	/bin/rm -rf
LDFLAGS	=	-lreadline
HEADER	=	includes/Pratt.h

all:		$(NAME)

$(NAME):	$(OBJS)
		$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $(NAME)

$(OBJ_DIR)/%.o:	$(SRC_DIR)/%.c $(HEADER)
		@mkdir -p $(OBJ_DIR)
		$(CC) $(CFLAGS) -c $< -o $@

debug:		$(OBJS)
		$(CC) $(CFLAGS) $(DEBUG) $(OBJS) $(LDFLAGS) -o $(NAME)

clean:
		$(RM) $(OBJ_DIR)

fclean: clean
		$(RM) $(NAME)

re:			fclean all

.PHONY:		all clean fclean re
