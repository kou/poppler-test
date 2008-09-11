#!/bin/sh

export BASE_DIR="`dirname $0`"

if test x"$NO_MAKE" != x"yes"; then
    make -C $BASE_DIR/../../ > /dev/null || exit 1
fi

if test -z "$CUTTER"; then
    CUTTER="`make -s -C $BASE_DIR echo-cutter`"
fi

CUTTER_WRAPPER=
if test x"$CUTTER_DEBUG" = x"yes"; then
    CUTTER_WRAPPER="$BASE_DIR/../../libtool --mode=execute gdb --args"
fi

CUTTER_ARGS="-s $BASE_DIR --exclude-directory lib"
CUTTER_ARGS="$CUTTER_ARGS --exclude-directory fixtures"
$CUTTER_WRAPPER $CUTTER $CUTTER_ARGS "$@" $BASE_DIR
