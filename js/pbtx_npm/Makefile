all: pbtx_pb.js


pbtx_pb.js: ../../pbtx.proto
	protoc -I../.. pbtx.proto --js_out=import_style=commonjs,binary:.

clean:
	rm -rf pbtx_pb.js node_modules/
