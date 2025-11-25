# Compilador e flags
CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lm

# Executáveis
TARGET1 = healthdata
TARGET2 = controle_termico

# Fontes
SRC1 = healthdata.c
SRC2 = main_ct.c controle_termico.c

# Regra padrão
all: $(TARGET1) $(TARGET2)

# healthdata
$(TARGET1): $(SRC1)
	$(CC) $(CFLAGS) -o $(TARGET1) $(SRC1) $(LDFLAGS)

# controle_termico com main_ct + controle_termico
$(TARGET2): $(SRC2)
	$(CC) $(CFLAGS) -o $(TARGET2) $(SRC2) $(LDFLAGS)

clean:
	rm -f $(TARGET1) $(TARGET2) *.o
