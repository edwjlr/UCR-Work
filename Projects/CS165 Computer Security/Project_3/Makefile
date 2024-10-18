#
# File          : Makefile
# Description   : Build file for CMPSC447 project 1, applied cryptography
#                 


# Environment Setup
#LIBDIRS=-L. -L/opt/local/lib
#INCLUDES=-I. -I/opt/local/include
#CC=gcc 
CC=clang
CFLAGS=-c $(INCLUDES) -g -Wall -m32 
# CFLAGS=-c $(INCLUDES) -g -Wall 
LINK=clang -g
LDFLAGS=$(LIBDIRS) -m32 
# LDFLAGS=$(LIBDIRS)
AR=ar rc
RANLIB=ranlib

# Suffix rules
.c.o :
	${CC} ${CFLAGS} $< -o $@

#
# Setup builds

TARGETS=cs165-p3
#LIBS=-lcrypto -lm
LIBS=

#
# Project Protections

p1 : $(TARGETS)

cmpsc447-p1 : cs165-p3.o
	$(LINK) $(LDFLAGS) cs165-p3.o $(LIBS) -o $@

clean:
	rm -f *.o *~ $(TARGETS)

BASENAME=p3
tar: 
	tar cvfz $(BASENAME).tgz -C ..\
	    $(BASENAME)/Makefile \
            $(BASENAME)/cs165-p3.c \
	    $(BASENAME)/cs165-p3.h \
	    $(BASENAME)/test \
	    $(BASENAME)/test2 \
	    $(BASENAME)/test3 \


