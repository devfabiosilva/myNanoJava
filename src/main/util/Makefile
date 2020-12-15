#
#	AUTHOR: Fábio Pereira da Silva
#	YEAR: 2020
#	LICENSE: MIT
#	EMAIL: fabioegel@gmail.com or fabioegel@protonmail.com
#

# Generating tools
#dom dez 06 16:49:52 -03 2020 
CC=gcc
STRIP=strip
EXECSTACK=execstack -c
INCLUDEDIR=$(PWD)/include/
INCLUDEDIRPRIVATE=$(PWD)/include/sodium/
LIBANAME=mynanojava
CURDIR=$(PWD)
LIBDIR=$(CURDIR)/lib
JAVAINCLUDE=/usr/lib/jvm/java-11-openjdk-amd64/include
JAVAINCLUDE_LINUX=/usr/lib/jvm/java-11-openjdk-amd64/include/linux

all: main

main: $(LIBANAME).c
	@echo "Compiling mynanojava ..."
	@$(CC) -O2 -fPIC -shared -o lib$(LIBANAME).so $(LIBANAME).c -I$(INCLUDEDIR) -I$(INCLUDEDIRPRIVATE) -I$(JAVAINCLUDE) -I$(JAVAINCLUDE_LINUX) -L$(LIBDIR) -lnanocrypto1 -lsodium
	@echo "Striping $(LIBANAME).so ..."
	@$(STRIP) lib$(LIBANAME).so
	@echo "Disabling execstack ..."
	@$(EXECSTACK) lib$(LIBANAME).so
	@echo "Unchecking executable library ..."
	@chmod -x lib$(LIBANAME).so
	@echo "Finished"

.PHONY: clean
clean:
	@echo "Cleaning $(LIBANAME) library..."
	rm -v lib$(LIBANAME).so

