#!/bin/sh
gcc main.c -o out `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0`
