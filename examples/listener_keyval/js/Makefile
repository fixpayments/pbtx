all: pb/keyval_pb.js  npminstall


pb/keyval_pb.js: ../keyval.proto
	protoc -I.. keyval.proto --js_out=import_style=commonjs,binary:pb


npminstall:
	npm install

clean:
	rm -rf pb/*.js node_modules/
