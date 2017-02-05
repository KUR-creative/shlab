# Makefile for the CS:APP Shell Lab

TEAM = NOBODY
VERSION = 1
HANDINDIR = /afs/cs/academic/class/15213-f02/L5/handin
DRIVER = ./sdriver.pl
TSH = ./tsh
TSHREF = ./tshref
TSHARGS = "-p"
CC = gcc
#CFLAGS = -Wall -O2
FILES = $(TSH) ./myspin ./mysplit ./mystop ./myint

LIBS = -lpthread  -lrt
INCS = -I./
CFLAGS = -g -Wall -std=gnu99 -O0  #-Werror
#CFLAGS = -g -Wall -std=gnu99 -O2  #-Werror

all: $(FILES)

##################
# Handin your work
##################
handin:
	cp tsh.c $(HANDINDIR)/$(TEAM)-$(VERSION)-tsh.c

tc: tsh.c
	$(CC) $(CFLAGS) -o tsh tsh.c csapp.c $(LIBS) $(INCS) -lcriterion
	./tsh

t: tsh.c
	$(CC) $(CFLAGS) -o tsh tsh.c csapp.c $(LIBS) $(INCS) -DRELEASE
	./tsh

t12: tsh.c
	$(CC) $(CFLAGS) -o tsh tsh.c csapp.c $(LIBS) $(INCS) -DRELEASE
	# -------------------------------1--------------------------------
	######## ref ########
	make rtest01
	######## mine ########
	make test01
	# -------------------------------2--------------------------------
	######## ref ########
	make rtest02
	######## mine ########
	make test02

t3: tsh.c
	$(CC) $(CFLAGS) -o tsh tsh.c csapp.c $(LIBS) $(INCS) -DRELEASE
	# -------------------------------3--------------------------------
	######## ref ########
	make rtest03
	######## mine ########
	make test03

t4: tsh.c
	$(CC) $(CFLAGS) -o tsh tsh.c csapp.c $(LIBS) $(INCS) -DRELEASE
	# -------------------------------4--------------------------------
	######## ref ########
	make rtest04
	######## mine ########
	make test04

t5: tsh.c
	$(CC) $(CFLAGS) -o tsh tsh.c csapp.c $(LIBS) $(INCS) -DRELEASE
	# -------------------------------5--------------------------------
	######## ref ########
	make rtest05
	######## mine ########
	make test05

t6: tsh.c
	$(CC) $(CFLAGS) -o tsh tsh.c csapp.c $(LIBS) $(INCS) -DRELEASE
	# -------------------------------5--------------------------------
	######## ref ########
	make rtest06
	######## mine ########
	make test06

t7: tsh.c
	$(CC) $(CFLAGS) -o tsh tsh.c csapp.c $(LIBS) $(INCS) -DRELEASE
	# -------------------------------5--------------------------------
	######## ref ########
	make rtest07
	######## mine ########
	make test07

tsmoke: tsh.c
	$(CC) $(CFLAGS) -o tsh tsh.c csapp.c $(LIBS) $(INCS) -DRELEASE
	#smoke test
	make rtest01
	make test01
	make rtest02
	make test02
	make rtest03
	make test03
	make rtest04
	make test04
	make rtest05
	make test05
	make rtest06
	make test06
	make rtest07
	make test07 

t8: tsh.c
	$(CC) $(CFLAGS) -o tsh tsh.c csapp.c $(LIBS) $(INCS) -DRELEASE
	# -------------------------------5--------------------------------
	######## ref ########
	make rtest08
	######## mine ########
	make test08

t9: tsh.c
	$(CC) $(CFLAGS) -o tsh tsh.c csapp.c $(LIBS) $(INCS) -DRELEASE
	# -------------------------------5--------------------------------
	######## ref ########
	make rtest09
	######## mine ########
	make test09

t16: tsh.c
	$(CC) $(CFLAGS) -o tsh tsh.c csapp.c $(LIBS) $(INCS) -DRELEASE
	# -------------------------------5--------------------------------
	######## ref ########
	make rtest16
	######## mine ########
	make test16

tp1: tsh.c
	$(CC) $(CFLAGS) -o tsh tsh.c csapp.c $(LIBS) $(INCS) -DRELEASE
	# -------------------------------5--------------------------------
	######## ref ########
	$(DRIVER) -t trp1.txt -s $(TSHREF) -a $(TSHARGS) 
	######## mine ########
	$(DRIVER) -t trp1.txt -s $(TSH) -a $(TSHARGS) 

tsh-release: tsh.c
	$(CC) $(CFLAGS) -o tsh tsh.c -DRELEASE
	

##################
# Regression tests
##################

# Run tests using the student's shell program
test01:
	$(DRIVER) -t trace01.txt -s $(TSH) -a $(TSHARGS) 
test02:
	$(DRIVER) -t trace02.txt -s $(TSH) -a $(TSHARGS) 
test03:
	$(DRIVER) -t trace03.txt -s $(TSH) -a $(TSHARGS) 
test04:
	$(DRIVER) -t trace04.txt -s $(TSH) -a $(TSHARGS) 
test05:
	$(DRIVER) -t trace05.txt -s $(TSH) -a $(TSHARGS) 
test06:
	$(DRIVER) -t trace06.txt -s $(TSH) -a $(TSHARGS) 
test07:
	$(DRIVER) -t trace07.txt -s $(TSH) -a $(TSHARGS) 
test08:
	$(DRIVER) -t trace08.txt -s $(TSH) -a $(TSHARGS) 
test09:
	$(DRIVER) -t trace09.txt -s $(TSH) -a $(TSHARGS) 
test10:
	$(DRIVER) -t trace10.txt -s $(TSH) -a $(TSHARGS) 
test11:
	$(DRIVER) -t trace11.txt -s $(TSH) -a $(TSHARGS) 
test12:
	$(DRIVER) -t trace12.txt -s $(TSH) -a $(TSHARGS) 
test13:
	$(DRIVER) -t trace13.txt -s $(TSH) -a $(TSHARGS) 
test14:
	$(DRIVER) -t trace14.txt -s $(TSH) -a $(TSHARGS) 
test15:
	$(DRIVER) -t trace15.txt -s $(TSH) -a $(TSHARGS) 
test16:
	$(DRIVER) -t trace16.txt -s $(TSH) -a $(TSHARGS) -v

# Run the tests using the reference shell program
rtest01:
	$(DRIVER) -t trace01.txt -s $(TSHREF) -a $(TSHARGS) 
rtest02:
	$(DRIVER) -t trace02.txt -s $(TSHREF) -a $(TSHARGS) 
rtest03:
	$(DRIVER) -t trace03.txt -s $(TSHREF) -a $(TSHARGS) 
rtest04:
	$(DRIVER) -t trace04.txt -s $(TSHREF) -a $(TSHARGS) 
rtest05:
	$(DRIVER) -t trace05.txt -s $(TSHREF) -a $(TSHARGS) 
rtest06:
	$(DRIVER) -t trace06.txt -s $(TSHREF) -a $(TSHARGS) 
rtest07:
	$(DRIVER) -t trace07.txt -s $(TSHREF) -a $(TSHARGS) 
rtest08:
	$(DRIVER) -t trace08.txt -s $(TSHREF) -a $(TSHARGS) 
rtest09:
	$(DRIVER) -t trace09.txt -s $(TSHREF) -a $(TSHARGS) 
rtest10:
	$(DRIVER) -t trace10.txt -s $(TSHREF) -a $(TSHARGS) 
rtest11:
	$(DRIVER) -t trace11.txt -s $(TSHREF) -a $(TSHARGS) 
rtest12:
	$(DRIVER) -t trace12.txt -s $(TSHREF) -a $(TSHARGS) 
rtest13:
	$(DRIVER) -t trace13.txt -s $(TSHREF) -a $(TSHARGS) 
rtest14:
	$(DRIVER) -t trace14.txt -s $(TSHREF) -a $(TSHARGS) 
rtest15:
	$(DRIVER) -t trace15.txt -s $(TSHREF) -a $(TSHARGS) 
rtest16:
	$(DRIVER) -t trace16.txt -s $(TSHREF) -a $(TSHARGS) -v


# clean up
clean:
	rm -f $(FILES) *.o *~


