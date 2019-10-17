#!/usr/bin/bash

failedTests=0

for f in tests/*.calc; do
    filename=$(basename -- "$f")
    extension="${filename##*.}"
    filename="${filename%.*}"
    `ocamlc -o interpreter str.cma $1`
    if [ $? != 0 ]; then
        tput setaf 1 ; echo "[Failure] $f - Compilation Error" ; tput sgr0 ; failedTests+=1 ; continue
    fi
    `./interpreter $f tests/$filename.input > tmp.out`
    diff tmp.out tests/$filename.output
    if [ $? -eq 0 ]; then
        tput setaf 2 ; echo "[Success] $f" ; tput sgr0
    else
         tput setaf 1 ; echo "[Failure] $f - Bad Output" ; tput sgr0 ; failedTests+=1
    fi
    rm -f interpreter.cmi interpreter interpreter.cmo
done;

rm tmp.out

exit $failedTests
