'use strict';

const pbtx = require('./pbtx_pb');
const EC = require('elliptic').ec;
const { PublicKey, SerialBuffer } = require('eosjs');
const { JsSignatureProvider } = require('eosjs/dist/eosjs-jssig');

const defaultEc = new EC('secp256k1');

class PBTX {

    // takes permission attributes
    // returns a Permission protobuf object
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

    
    // takes TransactionBody attributes
    // returns a Permission protobuf object
    makeTransactionBody(data) {
        let tb = new pbtx.pbtx.TransactionBody();
        
        if( data.network_id == null || typeof(data.network_id) !== 'bigint' ) {
            throw Error('network_id must be a BigInt');
        }

        tb.setNetworkId(data.network_id);

        if( data.actor == null || typeof(data.actor) !== 'bigint' ) {
            throw Error('actor must be a BigInt');
        }

        tb.setActor(data.actor);

        if( data.cosignors ) {
            if( !Array.isArray(data.cosignors) ) {
                throw Error('cosignors must be an array');
            }

            data.cosignors.forEach( account => {
                if( typeof(account) !== 'bigint' ) {
                    throw Error('cosignors must be BigInt');
                }
                tb.addCosignor(account);
            });
        }                   
        
        if( !Number.isInteger(data.seqnum) || data.seqnum < 1 ) {
            throw Error('seqnum must be a positive integer');
        }

        tb.setSeqnum(data.seqnum);

        if( !Number.isInteger(data.tansaction_type) || data.tansaction_type < 0 ) {
            throw Error('tansaction_type must be an unsigned integer');
        }

        tb.setTtransactionType(data.tansaction_type);

        if( data.transaction_content ) {
            tb.setTransactionContent(data.transaction_content);
        }

        return tb;
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

    
    async sendTransaction(tx, api, contract, worker) {
        return api.transact(
            {
                actions:
                [
                    {
                        account: contract,
                        name: 'exectrx',
                        authorization: [{
                            actor: worker,
                            permission: 'active'} ],
                        data: {
                            worker: worker,
                            trx_input: tx.serializeBinary(),
                        },
                    }
                ]
            },
            {
                blocksBehind: 100,
                expireSeconds: 3600
            }
        );
    }
}



module.exports = PBTX;
