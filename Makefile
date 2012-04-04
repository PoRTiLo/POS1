#TODO:PRWEVOD DO JINEHO KODOVANI!!!!!!!!!!!!!!!!!!!!!
#makefile
  # KKO- Kodovani a komprese
  # 9.2.2012
  # Autor: Bc. Jaroslav Sendler, FIT, xsendl00(at)stud.fit.vutbr.cz
  # Prelozeno gcc ...
  #

# jmeno prekladaneho programu
PROGRAM=proj01

# jmeno knihovny
LIB=gif2bmp

# nazev projektu
PROJECT=kko.proj3.xsendl00

SRC=proj01.c
#HEAD=

CCM=g++
CCMFLAGS=-std=c++98 -Wall -pedantic -Wextra 
CFLAGS=-ansi -pedantic -Wall -Wextra -g -O
CC=gcc
all:  clean ${PROGRAM} 

${PROGRAM}: 
#	$(CCM) $(CCMFLAGS) $(SRC) $(HEAD) -o $@
	$(CC) $(CFLAGS) $(SRC) -o $@

clean:
	rm -f *.o ${PROGRAM}

zip:
	zip ${PROJECT}  ${SRC} ${HEAD} Makefile ${PROGRAM}.pdf
