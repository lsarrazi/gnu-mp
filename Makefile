EM=em++
MPFR=${HOME}/opt/lib/libmpfr.a
GMP=${HOME}/opt/lib/libgmp.a
INCLUDE=${HOME}/opt/include ./includes
NAME=dist/gnu-mp.js
FLAGS=-s NO_EXIT_RUNTIME=0 --bind --no-entry -O1 -s ASSERTIONS=1
RM=rm -rf
FILES= Float.cpp Utils.cpp bindings.cpp
SRC= $(addprefix ./src/,$(FILES))

all: $(NAME)

$(NAME): $(SRC)
	mkdir -p dist
	$(EM) $(SRC) $(MPFR) $(GMP) $(addprefix -I,$(INCLUDE)) -o $(NAME) $(FLAGS) -s MODULARIZE
	node scripts/patch_glue.js $(NAME)
	cp res/NodeAPI.js dist
	cp res/package.json dist

index.html:
	$(EM) $(SRC) $(MPFR) $(GMP) $(addprefix -I,$(INCLUDE)) -o dist/index.html $(FLAGS)
	node scripts/patch_glue.js dist/index.js

re: clean all

clean:
	$(RM) -r dist

idl:
	python /opt/emsdk/tools/webidl_binder.py mpfr.idl glue

.PHONY: re clean all idl