NAME    = minishell
CC      = gcc
CFLAGS  = #-Wall -Wextra -Werror
RM      = rm -f
RL_LIBS = -lreadline -lncurses

SRC = main.c src/env_data.c src/signals.c \
	lexer/lexer.c lexer/lexer_special.c lexer/lexer_utils.c \
	expander/expander.c expander/expander_quote.c expander/expander_utils.c expander/expander_variables.c \
	expander/expander_type.c \
	parser/parser.c parser/parser_valid.c parser/parser_redir.c \
	parser/parser_utils.c parser/parser_cmd.c parser/parser_free_utils.c \
	executor/executor.c executor/executor2.c executor/executor3.c  executor/executor4.c \
	executor/executor_pipe.c executor/executor_pipe_free_utils.c \
	executor/executor_heredoc.c executor/executor_heredoc_utils.c executor/executor_heredoc_utils2.c \
	builtin/cd.c builtin/cd_utils.c builtin/echo.c builtin/env.c builtin/exit.c builtin/pwd.c builtin/unset.c \
	builtin/export.c builtin/export_utils.c builtin/export_utils1.c builtin/export_utils2.c builtin/export_utils3.c \
	utils/utils.c utils/utils2.c utils/utils3.c utils/split.c

OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(CFLAGS) -o $@ $(OBJ) $(RL_LIBS)
	@echo "Done!"

%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@$(RM) $(OBJ)
	@echo "Cleaned!"

fclean: clean
	@$(RM) $(NAME)

re: fclean all

LEAKFLAGS=--leak-check=full \
          --show-leak-kinds=all \
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
