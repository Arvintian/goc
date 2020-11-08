INSTALL_PATH=$(HOME)/bin

build:
	gcc -o goc src/main.c

install: build
	cp goc $(INSTALL_PATH)/goc

clean:
	rm goc