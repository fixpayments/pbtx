'use strict';

const pbtx = require('./pbtx_pb');
const EC = require('elliptic').ec;
const { PublicKey, SerialBuffer } = require('eosjs');
const { JsSignatureProvider } = require('eosjs/dist/eosjs-jssig');

const defaultEc = new EC('secp256k1');

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

        data.keys.forEach( keyweight => {
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

    // gets TransactionBody object and array of private keys in string format
    // returns Transaction object
    signTransactionBody(body, privateKeys) {
        const serializedBody = body.serializeBinary();
        const digest = defaultEc.hash().update(serializedBody).digest();

        let tx = new pbtx.pbtx.Transaction();
        tx.setBody(serializedBody);

        privateKeys.forEach( key => {
            const priv = PrivateKey.fromString(key);
            const privElliptic = priv.toElliptic();
            const privateKey = PrivateKey.fromElliptic(privElliptic, priv.getType());
            const signature = privateKey.sign(digest, false);
            
            let buffer = eosjs.SerialBuffer();
            buffer.push(signature.type);
            buffer.push(signature.data);

            let sig = new pbtx.pbtx.Signature();
            sig.setType(pbtx.pbtx.KeyType.EOSIO_KEY);
            sig.addSigBytes(buffer);
            
            tx.addSignature(sig);
        });
                             
        return tx;
    }
}



module.exports = PBTX;
