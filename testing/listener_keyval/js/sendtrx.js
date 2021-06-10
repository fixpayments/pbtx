'use strict';

const program   = require('commander');
const config    = require('config');
const fetch     = require('node-fetch');

const { Api, JsonRpc, RpcError } = require('eosjs');
const { JsSignatureProvider } = require('eosjs/dist/eosjs-jssig');
const { TextEncoder, TextDecoder } = require('util');

const pbtx = require('./pb/pbtx_pb');
const keyval = require('./pb/keyval_pb');

const url        = config.get('url');
const pbtxacc    = config.get('pbtxacc');
const keyvalacc  = config.get('keyvalacc');
const workeracc  = config.get('workeracc');
const workerkey  = config.get('workerkey');

const sigProvider = new JsSignatureProvider([workerkey]);
const rpc = new JsonRpc(url, { fetch });
const api = new Api({rpc: rpc, signatureProvider: sigProvider,
                     textDecoder: new TextDecoder(), textEncoder: new TextEncoder()});

function parseCmd(value, dummyPrevious) {
    if (value != "set" && value != "del") {
        throw new commander.InvalidOptionArgumentError('Invalid command: ' + value);
    }
    return value;
}

program
  .command('perm <networkid> <permjson>')
  .description('Set an actor permission')
  .action((networkid, permjson) => {
    console.log('perm called');
  });

program
  .command('set <networkid> <key> <val>')
  .description('Set a key/value pair')
    .action((networkid, key, val) => {
    console.log('set called');
  });

program
  .command('del <networkid> <key>')
  .description('Delete a key/value pair')
    .action((networkid, key) => {
    console.log('del called');
  });

program.parse(process.argv);


          
async function sendtrx()
{
    
}    


async function exectrx(trx_input) {
    try {
        const result = await api.transact(
            {
                actions:
                [
                    {
                        account: pbtxacc,
                        name: 'exectrx',
                        authorization: [{
                            actor: workeracc,
                            permission: 'active'} ],
                        data: {
                            worker: workeracc,
                            trx_input: trx_input
                        },
                    }
                ]
            },
            {
                blocksBehind: 100,
                expireSeconds: 600
            }
        );
        console.info('transaction ID: ', result.transaction_id);
    } catch (e) {
        console.error('ERROR: ' + e);
    }
}

