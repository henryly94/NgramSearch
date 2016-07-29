#!/bin/bash
make
head clean_vocab -n ${1} | cat > testfile
./main ${1}
