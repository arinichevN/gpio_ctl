#!/bin/bash

APP=gpio_ctl
APP_DBG=`printf "%s_dbg" "$APP"`
INST_DIR=/usr/sbin

MODE_DEBUG=-DMODE_DEBUG

source lib/gpio/cpu.sh
source lib/gpio/pinout.sh

NONE=""

function move_bin {
	([ -d $INST_DIR ] || mkdir $INST_DIR) && \
	cp $APP $INST_DIR/$APP && \
	chmod a+x $INST_DIR/$APP && \
	chmod og-w $INST_DIR/$APP && \
	echo "Your $APP executable file: $INST_DIR/$APP. Launch command: sudo $APP";
}
function build_lib {
	gcc $1 $2 -c app.c && \
	gcc $1 $2 -c timef.c && \
	gcc $1 $2 $PINOUT -c gpio.c && \

	echo "library: making archive..." && \
	rm -f libpac.a
	ar -crv libpac.a app.o timef.o gpio.o && \
	echo "library: done" && echo "hardware: $CPU $PINOUT"
	
	rm -f *.o acp/*.o
}

function build {
	cd lib && \
	build_lib $1 $2 && \
	cd ../ && \
	gcc -D_REENTRANT $1 $2 main.c -o $APP -lpthread -L./lib -lpac && \
	echo "Application successfully compiled."
}
function full {
	build $CPU $NONE && \
	move_bin
}
function debug {
	build $CPU $MODE_DEBUG
}
function uninstall {
	pkill -n $APP
	rm -v $INST_DIR/$APP
	rm -v $INST_DIR/$APP_DBG
}

f=$1
${f}