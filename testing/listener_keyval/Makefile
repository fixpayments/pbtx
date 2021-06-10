CONTRACT=keyval
NANOPB=../../nanopb
PROTODIR=.

all: $(CONTRACT).wasm


keyval.wasm: keyval.cpp keyval.pb.c keyval.pb.h
	eosio-cpp --no-missing-ricardian-clause -I. -I$(NANOPB) -o $@ keyval.cpp keyval.pb.c $(NANOPB)/pb_common.c $(NANOPB)/pb_decode.c

keyval.pb.c keyval.pb.h: $(PROTODIR)/keyval.proto $(PROTODIR)/keyval.options
	python3 $(NANOPB)/generator/nanopb_generator.py -I$(PROTODIR) keyval.proto

clean:
	rm -f $(CONTRACT).wasm $(CONTRACT).abi keyval.pb.c keyval.pb.h
