EM=em++
CPP=clang++
MPFR=${HOME}/opt/lib/libmpfr.a
GMP=${HOME}/opt/lib/libgmp.a
INCLUDE=${HOME}/opt/include ./includes
FLAGS=-s NO_EXIT_RUNTIME=0 --bind --no-entry -O3 -s ASSERTIONS=1 -s ALLOW_MEMORY_GROWTH=1
RM=rm -rf
FILES= Float.cpp FloatRegister.cpp Utils.cpp bindings.cpp quadrature/TanhSinh.cpp
SRC= $(addprefix ./src/,$(FILES))

all: dist

dist: dist/npm dist/web

dist/npm: dist/gnu-mp.js
	mkdir -p dist/npm
	cp dist/gnu-mp.js dist/npm
	cp dist/gnu-mp.wasm dist/npm
	cp res/NodeAPI.js dist/npm
	cp res/package.json dist/npm
	cp res/readme.md dist/npm

dist/web:
	mkdir -p dist/web
	cp dist/gnu-mp.js dist/web
	cp dist/gnu-mp.wasm dist/web
	cp res/WebAPI.js dist/web

dist/gnu-mp.js: $(SRC)
	mkdir -p dist
	$(EM) $(SRC) $(MPFR) $(GMP) $(addprefix -I,$(INCLUDE)) -o dist/gnu-mp.js $(FLAGS) -s MODULARIZE
	node scripts/patch_glue.js dist/gnu-mp.js

index.html:
	$(EM) $(SRC) $(MPFR) $(GMP) $(addprefix -I,$(INCLUDE)) -o dist/index.html $(FLAGS)
	node scripts/patch_glue.js dist/index.js

publish_npm: dist/npm
	npm publish dist/npm

re: clean all

clean:
	$(RM) -r dist

idl:
	python /opt/emsdk/tools/webidl_binder.py mpfr.idl glue

.PHONY: re clean all idl publish_npm