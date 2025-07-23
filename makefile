# Project name
NAME = minishell

# Compiler and flags
CC = cc
CFLAGS = -Wall -Wextra -Werror -Iinclude -Ilibft -g3
LDFLAGS = -lreadline

# Libft
LIBFT_DIR = libft
LIBFT = $(LIBFT_DIR)/libft.a

# Colors
GREEN = \033[0;32m
RESET = \033[0m

# Source files
SRC = main.c \
      debugin.c \
      lexer/full_lexeer.c \
      check_syntax/syntax_check_utils.c \
      check_syntax/helper.c \
      check_syntax/check_syntax.c \
      check_syntax/initial_syntax.c \
      check_syntax/validate_syntax.c \
      check_syntax/redirection_error.c \
      parser/full_parser.c \
      clean_up/ft_clean.c \
      execution/execute.c \
      expand/full_expande.c 

# Object files
OBJ = $(SRC:.c=.o)

# Default target
all: $(NAME)

$(NAME): $(OBJ) $(LIBFT)
	@echo "$(GREEN)Building $(NAME)...$(RESET)"
	@$(CC) $(CFLAGS) $(OBJ) $(LIBFT) $(LDFLAGS) -o $(NAME)
	@echo "$(GREEN)Build complete!$(RESET)"

# Compile libft
$(LIBFT):
	@$(MAKE) -C $(LIBFT_DIR)

# Clean object files
clean:
	@rm -f $(OBJ)
	@$(MAKE) -C $(LIBFT_DIR) clean

# Clean everything
fclean: clean
	@rm -f $(NAME)
	@$(MAKE) -C $(LIBFT_DIR) fclean

# Rebuild everything from scratch
re: fclean all

.PHONY: all clean fclean re
