#!/bin/sh

export BASE_DIR="`dirname $0`"
top_dir="$BASE_DIR/../.."

if test x"$NO_MAKE" != x"yes"; then
    make -C "$top_dir" > /dev/null || exit 1
fi

if test -z "$CUTTER"; then
    CUTTER="`make -s -C $BASE_DIR echo-cutter`"
fi

CUTTER_WRAPPER=
CUTTER_ARGS=
if test x"$CUTTER_DEBUG" = x"yes"; then
    CUTTER_WRAPPER="$top_dir/libtool --mode=execute gdb --args"
    CUTTER_ARGS="--keep-opening-modules"
fi

if test x"$CUTTER_VERBOSE" != x"no"; then
    CUTTER_ARGS="$CUTTER_ARGS --verbose=verbose"
fi

export CUTTER

CUTTER_ARGS="$CUTTER_ARGS -s $BASE_DIR"
CUTTER_ARGS="$CUTTER_ARGS --exclude-directory lib"
CUTTER_ARGS="$CUTTER_ARGS --exclude-directory fixtures"
$CUTTER_WRAPPER $CUTTER $CUTTER_ARGS "$@" $BASE_DIR
