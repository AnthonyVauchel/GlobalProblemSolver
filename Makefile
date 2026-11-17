CFLAGS = -Wall -Wextra -std=c11 -g -Iinclude
CC = gcc 
NAME = gps
SRC = src/main.c src/parser.c src/solver.c src/state.c src/display.c
OBJ = $(SRC:.c=.o)
GEN = generateur
 
all: $(NAME) $(GEN)
 
$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^
 
$(GEN): tools/generateur.c
	$(CC) $(CFLAGS) -o $@ $<
 
%.o: %.c include/GPS.h
	$(CC) $(CFLAGS) -c $< -o $@
 
clean:
	rm -f $(OBJ)
 
fclean: clean
	rm -f $(NAME) $(GEN)

re: fclean all