PBTX: Protocol Buffers Transaction protocol
===========================================

The goal of the project is to provide a base protocol for secure
mobile transactions, aiming for the following goals:

* Cryptographically protected security: every rtansaction is signed by
  the actor's private keys, and the protocol maintains the association
  of public keys and actors.

* Built-in integrity: every transaction has a sequence number, and the
  protocol keeps track of the last sequence number for every actor. It
  only accepts a next transaction with sequence number incremented by
  one.

* Low bandwidth: the transactions are compact and require only
  moinimal communication between the clients and servers. TRansactions
  can also be accumulated in offline storage and pushed to the network
  any time later.

* Built-in multi-signature: a transaction can be signed by multiple
  public keys.

* Universal base protocol: PBTX does not interpret the transaction
  content, and leaves its interpretation to the network-specific
  services. The protocol also allows multiple networks to use the same
  transport infrastructure.


EOSIO blockchain backend
------------------------

Currently EOSIO blockckchain is selected as the primary backend for
the protocol. It provides a smart contract that registers networks and
actors, and validates arriving transactions. It sends notifications to
network-specific listeners for further processing of the transaction
content.


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

