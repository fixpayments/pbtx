/*
  Copyright 2021 Fix Payments Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

'use strict';

const pbtx_pb = require('./pbtx_pb');
const EC = require('elliptic').ec;
const { PrivateKey } = require('eosjs/dist/PrivateKey');
const { SerialBuffer } = require('eosjs/dist/eosjs-serialize');
const { publicKeyToLegacyString, stringToPublicKey } = require('eosjs/dist/eosjs-numeric');

const defaultEc = new EC('secp256k1');


class PBTX {

    // takes permission attributes
    // returns a Permission protobuf object
    static makePermission(data) {
        let actor = BigInt(data.actor);

        if( !Number.isInteger(data.threshold) ) {
            throw Error('threshold must be an integer');
        }

        if( data.threshold == 0 ) {
            throw Error('threshold must be a positive integer');
        }

        if( !Array.isArray(data.keys) ) {
            throw Error('keys must be an array');
        }


        let perm = new pbtx_pb.Permission();
        perm.setActor(actor.toString());
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

            let buffer = new SerialBuffer();
            buffer.pushPublicKey(keyweight.key);

            let pk = new pbtx_pb.PublicKey();
            pk.setType(pbtx_pb.KeyType.EOSIO_KEY);
            pk.setKeyBytes(buffer.asUint8Array());

            let kw = new pbtx_pb.KeyWeight();
            kw.setKey(pk);
            kw.setWeight(keyweight.weight);

            perm.addKeys(kw);
        });

        return perm;
    }


    static permissionToObject(perm) {
        let data = {
            actor: perm.getActor(),
            threshold: perm.getThreshold(),
            keys: new Array()
        };

        perm.getKeysList().forEach(keyweight => {
            let key = keyweight.getKey();
            if( key.getType() != pbtx_pb.KeyType.EOSIO_KEY ) {
                throw Error('unsupported key type: ' + key.getType());
            }
            let buffer = new SerialBuffer()
            buffer.pushArray(key.getKeyBytes());

            const keyNewFormat = buffer.getPublicKey();

            data.keys.push({weight: keyweight.getWeight(),
                            key: publicKeyToLegacyString(stringToPublicKey(keyNewFormat))});
        });

        return data;
    }


    // takes TransactionBody attributes
    // returns a Permission protobuf object
    static makeTransactionBody(data) {
        let tb = new pbtx_pb.TransactionBody();

        if( data.network_id == null || typeof(data.network_id) !== 'bigint' ) {
            throw Error('network_id must be a BigInt');
        }

        tb.setNetworkId(data.network_id.toString());

        if( data.actor == null || typeof(data.actor) !== 'bigint' ) {
            throw Error('actor must be a BigInt');
        }

        tb.setActor(data.actor.toString());

        if( data.cosignors ) {
            if( !Array.isArray(data.cosignors) ) {
                throw Error('cosignors must be an array');
            }

            data.cosignors.forEach( account => {
                if( typeof(account) !== 'bigint' ) {
                    throw Error('cosignors must be BigInt');
                }
                tb.addCosignor(account.toString());
            });
        }

        if( !Number.isInteger(data.seqnum) || data.seqnum < 1 ) {
            throw Error('seqnum must be a positive integer: ' + data.seqnum);
        }

        tb.setSeqnum(data.seqnum);

        if( !Number.isInteger(data.transaction_type) || data.transaction_type < 0 ) {
            throw Error('transaction_type must be an unsigned integer: ' + data.transaction_type);
        }

        tb.setTransactionType(data.transaction_type);

        if( data.transaction_content ) {
            tb.setTransactionContent(data.transaction_content);
        }

        return tb;
    }



    // gets TransactionBody object and array of private keys in string format
    // returns Transaction object
    static signTransactionBody(body, privateKeys) {
        const serializedBody = body.serializeBinary();
        const digest = defaultEc.hash().update(serializedBody).digest();

        let tx = new pbtx_pb.Transaction();
        tx.setBody(serializedBody);

        privateKeys.forEach( key => {
            const priv = PrivateKey.fromString(key);
            const signature = priv.sign(digest, false);

            let buffer = new SerialBuffer();
            buffer.push(signature.signature.type);
            buffer.pushArray(signature.signature.data);

            let sig = new pbtx_pb.Signature();
            sig.setType(pbtx_pb.KeyType.EOSIO_KEY);
            sig.addSigBytes(buffer.asUint8Array());

            tx.addSignatures(sig);
        });

        return tx;
    }


    static async setPermission(network_id, perm, api, contract, admin) {
        return api.transact(
            {
                actions:
                [
                    {
                        account: contract,
                        name: 'regactor',
                        authorization: [{
                            actor: admin,
                            permission: 'active'} ],
                        data: {
                            network_id: network_id.toString(),
                            permission: perm.serializeBinary()
                        },
                    }
                ]
            },
            {
                blocksBehind: 100,
                expireSeconds: 3600
            });
    }


    static async sendTransaction(tx, api, contract, worker) {
        // console.log(Buffer.from(tx.serializeBinary()).toString('hex'));
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
                            trx_input: tx.serializeBinary()
                        },
                    }
                ]
            },
            {
                blocksBehind: 100,
                expireSeconds: 3600
            });
    }
}


module.exports = PBTX;
