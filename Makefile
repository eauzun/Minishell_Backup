NAME    = minishell
CC      = gcc
CFLAGS  = -g # -Wall -Wextra -Werror
RM      = rm -f

# OS'ye göre readline ayarları
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
  RL_INC ?= -I/opt/homebrew/opt/readline/include
  RL_LIB ?= -L/opt/homebrew/opt/readline/lib
  RL_LIBS = -lreadline
else
  RL_INC :=
  RL_LIB :=
  RL_LIBS = -lreadline -lncurses
endif

INC = -Isrc $(RL_INC)

SRC = main.c src/env_data.c src/signals.c src/cleanup.c \
		lexer/lexer.c lexer/lexer_special.c lexer/lexer_utils.c \
		expander/expander.c expander/expander_quote.c expander/expander_utils.c expander/expander_variables.c expander/expander_type.c \
		parser/parser.c parser/parser_valid.c parser/parser_redir.c parser/parser_utils.c parser/parser_cmd.c \
		executor/executor.c  executor/executor_pipe.c \
		builtin/cd.c builtin/cd_utils.c builtin/echo.c builtin/env.c builtin/exit.c builtin/pwd.c builtin/unset.c \
		builtin/export.c builtin/export_utils.c builtin/export_utils1.c builtin/export_utils2.c builtin/export_utils3.c \
		utils/utils.c utils/utils1.c utils/utils2.c utils/utils3.c \

OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(CFLAGS) -o $@ $(OBJ) $(RL_LIB) $(RL_LIBS)
	@echo "Done!"

%.o: %.c
	@$(CC) $(CFLAGS) $(INC) -c $< -o $@

clean:
	@$(RM) $(OBJ)
	@echo "Cleaned!"
fclean: clean
	@$(RM) $(NAME)

re: fclean all

LEAKFLAGS=--leak-check=full \
          --show-leak-kinds=definite,indirect \
          --track-origins=yes --track-fds=yes \
          --suppressions=readline.supp \
          --trace-children=no \
          --error-exitcode=99

leaks:
ifdef INPUT
	@echo "▶ Valgrind with INPUT: $(INPUT)"
	@[ -f "$(INPUT)" ] || { echo "ERROR: INPUT not found: $(INPUT)"; exit 2; }
	@valgrind $(LEAKFLAGS) ./$(NAME) < "$(INPUT)"
else
	@echo "▶ Valgrind (interactive)"
	@valgrind $(LEAKFLAGS) ./$(NAME)
endif

.PHONY: all clean fclean re
