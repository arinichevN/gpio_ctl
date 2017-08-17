#!/bin/bash

APP=gpio_ctl
APP_DBG=`printf "%s_dbg" "$APP"`
INST_DIR=/usr/sbin

MODE_DEBUG=-DMODE_DEBUG

#CPU=-DCPU_ANY
#CPU=-DCPU_ALLWINNER_A20
CPU=-DCPU_ALLWINNER_H3
#CPU=-DCPU_CORTEX_A5

PINOUT=-DPINOUT1
#PINOUT=-DPINOUT2

NONE=""

function move_bin {
	([ -d $INST_DIR ] || mkdir $INST_DIR) && \
	cp $APP $INST_DIR/$APP && \
	chmod a+x $INST_DIR/$APP && \
	chmod og-w $INST_DIR/$APP && \
	echo "Your $APP executable file: $INST_DIR/$APP";
}
function build_lib {
	gcc $1 $2 -c timef.c && \
	gcc $1 $2 $PINOUT -c gpio.c && \

	echo "library: making archive..." && \
	rm -f libpac.a
	ar -crv libpac.a timef.o gpio.o && \
	echo "library: done"
	rm -f *.o acp/*.o
}

function build {
	cd lib && \
	build_lib $1 $2 && \
	cd ../ && \
	gcc -D_REENTRANT $1 $2 main.c -o $APP -lpthread -L./lib -lpac && \
	echo "Application successfully compiled. Launch command: sudo ./"$APP
}
function full {
	build $CPU $NONE && \
	move_bin
}
function debug {
	build $CPU $MODE_DEBUG
}

f=$1
${f}