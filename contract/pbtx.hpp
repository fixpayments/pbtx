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

#include <eosio/eosio.hpp>
#include <eosio/multi_index.hpp>
#include <eosio/time.hpp>
#include "pbtx_contract_constants.hpp"

using namespace eosio;
using namespace std;



CONTRACT pbtx : public eosio::contract {
 public:

  pbtx( name self, name code, datastream<const char*> ds ):
    contract(self, code, ds)
    {}

  // register a new network (requires addmin_acc authentication)
  ACTION regnetwork(uint64_t network_id, name admin_acc, vector<name> listeners, uint32_t flags);

  // supply the network metadata in Protobuf encoding
  ACTION netmetadata(uint64_t network_id, vector<uint8_t> metadata);

  // delete a network (requires all actors to be deleted first)
  ACTION unregnetwork(uint64_t network_id);

  // add a new actor account to a network, specifying the credentials
  // in Permission protobuf message
  ACTION regactor(uint64_t network_id, vector<uint8_t> permission);

  // delete an actor from a network
  ACTION unregactor(uint64_t network_id, uint64_t actor);

  // execute the transaction
  ACTION exectrx(name worker, vector<uint8_t> trx_input);

  struct pbtxtransact_abi {
    name               worker;
    uint64_t           actor;
    uint32_t           seqnum;
    vector<uint64_t>   cosignors;
    uint32_t           transaction_type;
    vector<uint8_t>    transaction_content;
  };


  // cleans the history up to given ID
  ACTION cleanhistory(uint64_t network_id, uint64_t upto_id, uint32_t maxrows);
  
 private:

  // registry of networks and their admins, scope=0
  struct [[eosio::table("networks")]] networks_row {
    uint64_t           network_id;
    name               admin_acc;
    vector<name>       listeners;
    uint32_t           flags;
    auto primary_key()const { return network_id; }
  };

  typedef eosio::multi_index<name("networks"), networks_row> networks;


  // registry of network metadata, scope=0
  struct [[eosio::table("netmetadata")]] netmd_row {
    uint64_t           network_id;
    vector<uint8_t>    data;     // protobuf encoded metadata
    auto primary_key()const { return network_id; }
  };

  typedef eosio::multi_index<name("netmetadata"), netmd_row> netmd;

  // actors registry, scope=network_id
  struct [[eosio::table("actorperm")]] actorperm_row {
    uint64_t           actor;
    vector<uint8_t>    permission; // protobuf encoded Permission message
    auto primary_key()const { return actor; }
  };

  typedef eosio::multi_index<name("actorperm"), actorperm_row> actorperm;


  // actor seqence number and last transaction timestamp, scope=network_id
  struct [[eosio::table("actorseq")]] actorseq_row {
    uint64_t           actor;
    uint32_t           seqnum;     // sequence number. Only transactions with seqnum+1 are accepted
    uint64_t           prev_hash;   // previous body hash (first 64 bits from sha256 in big-endian)
    time_point         last_modified;
    auto primary_key()const { return actor; }
  };

  typedef eosio::multi_index<name("actorseq"), actorseq_row> actorseq;

  // last history row identifier, scope=0
  struct [[eosio::table("histid")]] histid_row {
    uint64_t           network_id;
    uint64_t           last_history_id;
    auto primary_key()const { return network_id; }
  };

  typedef eosio::multi_index<name("histid"), histid_row> histid;

  // history entries, scope=network_id
  struct [[eosio::table("history")]] history_row {
    uint64_t           id;
    uint8_t            event_type;
    vector<uint8_t>    data;       // protobuf encoded message
    checksum256        trx_id;     // EOSIO transaction ID and timestamp
    time_point         trx_time;
    auto primary_key()const { return id; }
  };

  typedef eosio::multi_index<name("history"), history_row> history;

  void add_history(uint64_t network_id, uint8_t event_type, vector<uint8_t>  data, name rampayer);
};
