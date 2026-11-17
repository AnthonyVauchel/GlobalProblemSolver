CC = gcc 
NAME = gps
SRC = main.c parser.c solver.c state.c display.c
OBJ = $(SRC:.c=.o)
GEN = generateur
 
all: $(NAME) $(GEN)
 
$(NAME): $(OBJ)
	$(CC) -o $@ $^
 
$(GEN): generateur.c
	$(CC) -o $@ $<
 
%.o: %.c GPS.h
	$(CC) $(CFLAGS) -c $< -o $@
 
clean:
	rm -f $(OBJ)
 
fclean: clean
	rm -f $(NAME) $(GEN)