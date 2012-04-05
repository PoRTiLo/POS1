#makefile
  # POS - Pokrocile operacni systemy
  # 4.5.2012
  # Autor: Bc. Jaroslav Sendler, FIT, xsendl00(at)stud.fit.vutbr.cz
  # Prelozeno gcc ...
  #

# jmeno prekladaneho programu
PROGRAM=proj01

SRC=proj01.c

CFLAGS=-ansi -pedantic -Wall -Wextra -g -O
CC=gcc
all:  clean ${PROGRAM} 

${PROGRAM}: 
	$(CC) $(CFLAGS) $(SRC) -o $@

clean:
	rm -f *.o ${PROGRAM}