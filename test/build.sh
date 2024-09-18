#!/usr/bin/env bash

find . -iname "*.java" | xargs javac --release 8 -Xlint:-options