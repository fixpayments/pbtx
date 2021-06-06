CONTRACT=pbtx
NANOPB=../../nanopb
PROTODIR=..

all: $(CONTRACT).wasm


pbtx.wasm: pbtx.cpp pbtx.hpp pbtx.pb.c pbtx.pb.h
	eosio-cpp --no-missing-ricardian-clause -I. -I$(NANOPB) -o $@ pbtx.cpp pbtx.pb.c $(NANOPB)/pb_common.c $(NANOPB)/pb_decode.c

pbtx.pb.c pbtx.pb.h: $(PROTODIR)/pbtx.proto
	python3 $(NANOPB)/generator/nanopb_generator.py -I$(PROTODIR) pbtx.proto

clean:
	rm -f $(CONTRACT).wasm $(CONTRACT).abi pbtx.pb.c pbtx.pb.h
