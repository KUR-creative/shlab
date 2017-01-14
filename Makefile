# Makefile for the CS:APP Shell Lab

TEAM = NOBODY
VERSION = 1
HANDINDIR = /afs/cs/academic/class/15213-f02/L5/handin
DRIVER = ./sdriver.pl
TSH = ./tsh
TSHREF = ./tshref
TSHARGS = "-p"
CC = gcc
CFLAGS = -Wall -O2
FILES = $(TSH) ./myspin ./mysplit ./mystop ./myint

all: $(FILES)

##################
# Handin your work
##################
handin:
	cp tsh.c $(HANDINDIR)/$(TEAM)-$(VERSION)-tsh.c

no:
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

t: tsh.c
	$(CC) $(CFLAGS) -o tsh tsh.c -DRELEASE
	# -------------------------------3--------------------------------
	######## ref ########
	make rtest03
	######## mine ########
	make test03


nt: tsh.c
	$(CC) $(CFLAGS) -o tsh tsh.c -lcriterion
	./tsh

tsh-release: tsh.c
	$(CC) $(CFLAGS) -o tsh tsh.c -DRELEASE
	

##################
# Regression tests
##################

# Run tests using the student's shell program
test01:
	$(DRIVER) -t trace01.txt -s $(TSH) -a $(TSHARGS) -v
test02:
	$(DRIVER) -t trace02.txt -s $(TSH) -a $(TSHARGS) -v
test03:
	$(DRIVER) -t trace03.txt -s $(TSH) -a $(TSHARGS) -v
test04:
	$(DRIVER) -t trace04.txt -s $(TSH) -a $(TSHARGS) -v
test05:
	$(DRIVER) -t trace05.txt -s $(TSH) -a $(TSHARGS) -v
test06:
	$(DRIVER) -t trace06.txt -s $(TSH) -a $(TSHARGS) -v
test07:
	$(DRIVER) -t trace07.txt -s $(TSH) -a $(TSHARGS) -v
test08:
	$(DRIVER) -t trace08.txt -s $(TSH) -a $(TSHARGS) -v
test09:
	$(DRIVER) -t trace09.txt -s $(TSH) -a $(TSHARGS) -v
test10:
	$(DRIVER) -t trace10.txt -s $(TSH) -a $(TSHARGS) -v
test11:
	$(DRIVER) -t trace11.txt -s $(TSH) -a $(TSHARGS) -v
test12:
	$(DRIVER) -t trace12.txt -s $(TSH) -a $(TSHARGS) -v
test13:
	$(DRIVER) -t trace13.txt -s $(TSH) -a $(TSHARGS) -v
test14:
	$(DRIVER) -t trace14.txt -s $(TSH) -a $(TSHARGS) -v
test15:
	$(DRIVER) -t trace15.txt -s $(TSH) -a $(TSHARGS) -v
test16:
	$(DRIVER) -t trace16.txt -s $(TSH) -a $(TSHARGS) -v

# Run the tests using the reference shell program
rtest01:
	$(DRIVER) -t trace01.txt -s $(TSHREF) -a $(TSHARGS) -v
rtest02:
	$(DRIVER) -t trace02.txt -s $(TSHREF) -a $(TSHARGS) -v
rtest03:
	$(DRIVER) -t trace03.txt -s $(TSHREF) -a $(TSHARGS) -v
rtest04:
	$(DRIVER) -t trace04.txt -s $(TSHREF) -a $(TSHARGS) -v
rtest05:
	$(DRIVER) -t trace05.txt -s $(TSHREF) -a $(TSHARGS) -v
rtest06:
	$(DRIVER) -t trace06.txt -s $(TSHREF) -a $(TSHARGS) -v
rtest07:
	$(DRIVER) -t trace07.txt -s $(TSHREF) -a $(TSHARGS) -v
rtest08:
	$(DRIVER) -t trace08.txt -s $(TSHREF) -a $(TSHARGS) -v
rtest09:
	$(DRIVER) -t trace09.txt -s $(TSHREF) -a $(TSHARGS) -v
rtest10:
	$(DRIVER) -t trace10.txt -s $(TSHREF) -a $(TSHARGS) -v
rtest11:
	$(DRIVER) -t trace11.txt -s $(TSHREF) -a $(TSHARGS) -v
rtest12:
	$(DRIVER) -t trace12.txt -s $(TSHREF) -a $(TSHARGS) -v
rtest13:
	$(DRIVER) -t trace13.txt -s $(TSHREF) -a $(TSHARGS) -v
rtest14:
	$(DRIVER) -t trace14.txt -s $(TSHREF) -a $(TSHARGS) -v
rtest15:
	$(DRIVER) -t trace15.txt -s $(TSHREF) -a $(TSHARGS) -v
rtest16:
	$(DRIVER) -t trace16.txt -s $(TSHREF) -a $(TSHARGS) -v


# clean up
clean:
	rm -f $(FILES) *.o *~


