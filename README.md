PBTX: Protocol Buffers Transaction protocol
===========================================

The goal of the project is to provide a base protocol for secure
mobile transactions, aiming for the following goals:

* Cryptographically protected security: every transaction is signed by
  the actor's private keys, and the protocol maintains the association
  of public keys and actors.

* Built-in integrity: every transaction has a sequence number, and the
  protocol keeps track of the last sequence number for every actor. It
  only accepts a next transaction with sequence number incremented by
  one.

* Low bandwidth: the transactions are compact and require only
  minimal communication between the clients and servers. Transactions
  can also be accumulated in offline storage and pushed to the network
  any time later.

* Built-in multi-signature: a transaction can be signed by multiple
  public keys.

* Universal base protocol: PBTX does not interpret the transaction
  content, and leaves its interpretation to the network-specific
  services. The protocol also allows multiple networks to use the same
  transport infrastructure.



EOSIO blockchain back-end
-------------------------

Currently EOSIO blockchain is selected as the primary back-end for the
protocol. It provides a smart contract that registers networks and
actors, and validates arriving transactions. It sends notifications to
network-specific listeners for further processing of the transaction
content.



Protobuf definition
-------------------

[pbtx.proto](pbtx.proto) defines the protocol data structures.



Protocol components
-------------------

### Networks

Network ID is a 64-bit integer that defines a scope of work for a
protocol: actors, permissions, transaction content format. The
protocol does not limit the number of networks, and there can
potentially be millions of independent networks.

Each network defines an admin account: an EOSIO account which can
modify the network settings and manage actors. The admin account is
also providing the RAM resource for contract tables.

Anyone can register a new network ID on the PBTX contract and become
its administrator.

### Actors

The concept of an actor is similar to that of an EOSIO account, but it is not associated with any EOSIO structures.

Each actor is identified by a 64-bit integer, and is associated with
one or multiple ECC public keys that are authorized to sign
transactions on actor's behalf.

Each actor is defined by a Permission protobuf object that is stored
in PBTX smart contract. The object defines a threshold and up to 16
public keys with their corresponding weights. A transaction should be
signed by a sufficient number of keys so that their sum of weights is
not less than the threshold.

Most of simple accounts would define the threshold of 1, and only one
public key. But others may be protected by a hardware wallet as a
second factor authentication, or some external escrow service
releasing the signatures.


### Co-signors

A transaction may require co-signing or endorsement of certain actions
by a third party. So, the transaction would list a number of
co-signors in addition to the actor signature.


### Transactions

A PBTX transaction is a protobuf message of Transaction type. It
consists of body and signatures.

The signatures field is a list of Signature messages, each
representing an actor or a co-signor in the order of their appearance
in the body. Each Signature message may contain several ECC signatures
necessary to satisfy the Permission threshold.

The body is an array of bytes where TransactionBody message is encoded
in Protobuf format, signed by those signatures.

The body message contains network ID, actor ID, optional cosignors,
sequence number, previous body's hash, transaction type and content.

The sequence number MUST be consecutive among actor's
transactions. The PBTX smart contract keeps track of last seen
sequence number, and is only accepting the number incremented by one.

The previous body's hash is the first 64 bits of sha256 hash of
previous transaction body, recorded into a 64-bit integer in
big-endian format. That is, the upper byte of the integer contains the
first byte of the hash. The `prev_hash` field in the new transaction
MUST match the hash of the previous transaction body.

Transaction type is a 32-bit integer that underlying payload
processors are free to interpret for their purpose.

Transaction content is a byte array containing the payload that is
specific to the network. It is supposed to be in protobuf format, but
not necessarily (for example, it may contain an EOSIO action to
execute).



### Workers

A worker is an EOSIO account calling the `exectrx` action on PBTX
contract, supplying the Transaction message as a byte array.

Theoretically any account is able to call this action and supply a
valid transaction message. But in many cases the listener contract
would need to charge the worker for RAM, and the worker account needs
a special permission that enables it.



### Listener contracts

Each network can define one or several Listener accounts. These
accounts are supposed to run smart contracts which would be called by
the PBTX smart contract after it validates a transaction message.

There are two ways of sending notifications, defined by the lowest bit
in network flags:

* by default, `pbtxtransact` action is called on the listener
  contracts. In this case, the listener contract can charge the worker
  for RAM if needed.

* if the lowest bit in flags is set, the listener contracts are
  notified by means of calling `require_recipient()` from the PBTX
  contract, so that they receive a full copy of the original
  Transaction message. In this case, they are unable to charge the
  worker for RAM.

The listener action should be defined as follows:

```
  ACTION pbtxtransact( name               worker,
                       uint64_t           actor,
                       uint32_t           seqnum,
                       vector<uint64_t>   cosignors,
                       uint32_t           transaction_type,
                       vector<uint8_t>    transaction_content )
```

The listener contract is implementing the actual logic of a PBTX
network. It defines what needs to be delivered in transaction type and
content, and keeps its own state that is relevant to the application.


Nodejs modules
--------------

The `js/pbtx_npm/` folder contains a nodejs module that helps
communicating with the PBTX contract. It allows setting permissions
and sending PBTX transactions by using a provided command-line
utility.


Example listener
----------------

`examples/listener_keyval/` contains a simple listener contract that
takes a protobuf message in transaction contents and sets or deletes
key/value pairs in its memory. It contains also a small nodejs
command-line program that generates the transaction payload for
passing as an argument in `pbtxutil` command.

[telos_testnet.txt](examples/listener_keyval/telos_testnet.txt)
documents the contract testing and deployment on Telos testnet.







Installation
------------

Dependencies

* eosio 2.0 compiled from sources
* eosio.cdt 1.6
* cmake 3.5
* python3-protobuf
* protobuf-compiler(protoc)

```
sudo apt install -y python3-protobuf protobuf-compiler cmake

git clone https://github.com/fixpayments/pbtx.git
cd pbtx
git submodule update --init --recursive

# specify the location of CDT with -c option
# If unit tests are needed, use -t flag and specify the location of compiled  eosio-2.0 with -e option
./build.sh -e /root/eosio/2.0 -c /usr -t
```

Deployoing the contract:

```
cleos set contract <your_account> ./build/Release/pbtx pbtx.wasm pbtx.abi
```


Copyright and License
=====================

Copyright 2021 Fix Payments Inc.

Licensed under the Apache License, Version 2.0 (the "License"); you
may not use this file except in compliance with the License.  You may
obtain a copy of the License at
[http://www.apache.org/licenses/LICENSE-2.0](http://www.apache.org/licenses/LICENSE-2.0)

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied.  See the License for the specific language governing
permissions and limitations under the License.

