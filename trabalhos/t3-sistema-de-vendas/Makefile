CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2

SRCS = util.c cliente.c produto.c venda.c arquivo.c relatorio.c main.c
OBJS = $(SRCS:.c=.o)
HDRS = util.h cliente.h produto.h venda.h arquivo.h relatorio.h

ifeq ($(OS),Windows_NT)
	TARGET = vendas.exe
	# mesmo esquema do T1: sem shell POSIX (cmd puro) o rm nao existe e vai del
	ifeq (,$(findstring /,$(SHELL))$(MSYSTEM))
		LIMPA = del /Q $(subst /,\,$(OBJS) $(TARGET))
	else
		LIMPA = rm -f $(OBJS) $(TARGET)
	endif
else
	TARGET = vendas
	LIMPA = rm -f $(OBJS) $(TARGET)
endif

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(LIMPA)

.PHONY: all clean
