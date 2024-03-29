# $Id: Makefile,v 1.18 2017-08-07 16:33:26-07 - - $

GPP        = g++ -g -O0 -Wall -Wextra -Wold-style-cast -std=gnu++14

DEPFILE    = Makefile.dep
HEADERS    = sockets.h logstream.h
CPPSRCS    = sockets.cpp client.cpp server.cpp
CLIENTOBJS = client.o sockets.o
SERVEROBJS = server.o sockets.o
OBJECTS    = ${CLIENTOBJS} ${SERVEROBJS}
EXECBINS   = client server
LISTING    = Listing.ps
SOURCES    = ${HEADERS} ${CPPSRCS} Makefile

all: ${DEPFILE} ${EXECBINS}

client: ${CLIENTOBJS}
	${GPP} -o $@ ${CLIENTOBJS}

server: ${SERVEROBJS}
	${GPP} -o $@ ${SERVEROBJS}

%.o: %.cpp
	${GPP} -c $<

ci:
	- checksource ${SOURCES}
	- cid + ${SOURCES}

lis: all ${SOURCES} ${DEPFILE}
	mkpspdf ${LISTING} ${SOURCES} ${DEPFILE}

clean:
	- rm ${LISTING} ${LISTING:.ps=.pdf} ${OBJECTS}

spotless: clean
	- rm ${EXECBINS}

dep:
	- rm ${DEPFILE}
	make --no-print-directory ${DEPFILE}

${DEPFILE}:
	${GPP} -MM ${CPPSRCS} >${DEPFILE}

again: ${SOURCES}
	make --no-print-directory spotless ci all lis

include ${DEPFILE}

