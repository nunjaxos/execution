# ================================== #
#           VARIABLES                #
# ================================== #

# Compiler & Flags
CC      = gcc
CFLAGS  = -Wall -Wextra -Werror
AR      = ar rcs
RM      = rm -f

# Project Structure
NAME    = libft.a
OBJ_DIR = obj/

# Source Files (Mandatory + Bonus)
SRCS    = ft_atoi.c ft_bzero.c ft_calloc.c ft_isalnum.c ft_isalpha.c \
          ft_isascii.c ft_isdigit.c ft_isprint.c ft_memchr.c ft_memcmp.c \
          ft_memmove.c ft_memset.c ft_strchr.c ft_strlen.c ft_strnstr.c \
          ft_strrchr.c ft_tolower.c ft_toupper.c ft_memcpy.c ft_strdup.c \
          ft_strlcat.c ft_strlcpy.c ft_strncmp.c ft_substr.c ft_putnbr_fd.c \
          ft_strjoin.c ft_strtrim.c ft_putchar_fd.c ft_putstr_fd.c \
          ft_putendl_fd.c ft_itoa.c ft_split.c ft_striteri.c ft_strmapi.c \
          get_next_line.c get_next_line_utils.c

BONUS   = ft_lstnew.c ft_lstsize.c ft_lstmap.c ft_lstlast.c ft_lstiter.c \
          ft_lstdelone.c ft_lstclear.c ft_lstadd_front.c ft_lstadd_back.c

# Object Files
OBJS        = $(addprefix $(OBJ_DIR), $(SRCS:.c=.o))
BONUS_OBJS  = $(addprefix $(OBJ_DIR), $(BONUS:.c=.o))

# Dependency Tracking (Auto-Update if Headers Change)
DEPS = $(OBJS:.o=.d) $(BONUS_OBJS:.o=.d)

# ================================== #
#           RULES                    #
# ================================== #

# Default Target (Mandatory Part)
all: $(NAME)

# Archive Library (Mandatory)
$(NAME): $(OBJS)
	$(AR) $@ $^

# Bonus Target
bonus: $(BONUS_OBJS)
	$(AR) $(NAME) $^

# Compile Objects + Generate Dependencies (FIXED: removed SRC_DIR)
$(OBJ_DIR)%.o: %.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

# Create obj/ Directory if Missing
$(OBJ_DIR):
	mkdir -p $@

# Clean Objects
clean:
	$(RM) -r $(OBJ_DIR)

# Clean Everything
fclean: clean
	$(RM) $(NAME)

# Rebuild
re: fclean all

# Include Dependencies (Auto-Generated)
-include $(DEPS)

.PHONY: all clean fclean re bonus