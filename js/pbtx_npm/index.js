'use strict';

const pbtx = require('./pbtx_pb');
const { PublicKey, SerialBuffer } = require('eosjs');
const { JsSignatureProvider } = require('eosjs/dist/eosjs-jssig');

class PBTX {

    makePermission(data) {
        if( !Number.isInteger(data.actor) ) {
            throw Error('actor must be an integer');
        }

        if( !Number.isInteger(data.threshold) ) {
            throw Error('threshold must be an integer');
        }

        if( data.threshold == 0 ) {
            throw Error('threshold must be a positive integer');
        }

        if( !Array.isArray(data.keys) ) {
            throw Error('keys must be an array');
        }


        let perm = new pbtx.pbtx.Permission();
        perm.setActor(data.actor);
        perm.setThreshold(data.threshold);

        data.keys.forEach(keyweight => {
            if( !Number.isInteger(keyweight.weight) ) {
                throw Error('key weight must be an integer');
            }

            if( keyweight.weight == 0 ) {
                throw Error('key weight must be a positive integer');
            }

            if( keyweight.key == null ) {
                throw Error('key must be defined');
            }

            let buffer = eosjs.SerialBuffer();
            buffer.pushPublicKey(keyweight.key);

            let pk = new pbtx.pbtx.PublicKey();
            pk.setType(pbtx.pbtx.KeyType.EOSIO_KEY);
            pk.setKeyBytes(buffer.array);

            let kw = new pbtx.pbtx.KeyWeight();
            kw.setKey(pk);
            kw.setWeight(keyweight.weight);

            perm.addKeys(kw);
        });

        return perm;
    }
}



module.exports = PBTX;
