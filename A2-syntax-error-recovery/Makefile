# Trivial makefile for the calculator scanner/parser.
# Depends on default (built-in) rules for C compilation.

# Note that rule for goal (parse) must be the first one in this file.

ifeq (test,$(firstword $(MAKECMDGOALS)))
  # use the rest as arguments for "test"
  TEST_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  .SILENT:
endif

parse: parse.o scan.o
	g++ -o parse parse.o scan.o

test:
	./parse < $(TEST_ARGS)

clean:
	rm *.o parse

parse.o: scan.h
scan.o: scan.h
