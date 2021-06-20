#include <eosio/eosio.hpp>
#include <eosio/multi_index.hpp>
#include "keyval.pb.h"
#include <pb_decode.h>

using namespace eosio;
using namespace std;



CONTRACT keyval : public eosio::contract {
 public:

  keyval( name self, name code, datastream<const char*> ds ):
    contract(self, code, ds)
    {}

  ACTION pbtxcontract( name pbtx_acc )
  {
    require_auth(_self);
    set_iprop(name("pbtxcontract"), pbtx_acc.value);
  }

  ACTION pbtxtransact( name               worker,
                       uint64_t           actor,
                       uint32_t           seqnum,
                       vector<uint64_t>   cosignors,
                       uint32_t           transaction_type,
                       vector<uint8_t>    transaction_content )
  {
    name pbtxcontract(get_iprop(name("pbtxcontract")));
    check(pbtxcontract.value != 0, "pbtxcontract is not set");
    require_auth(pbtxcontract);
    require_auth(worker);

    check(transaction_content.size() > 0, "empty transaction_content");

    keyval_Command* cmd = (keyval_Command*) malloc(sizeof(keyval_Command));
    pb_istream_t cmd_stream = pb_istream_from_buffer(transaction_content.data(), transaction_content.size());
    check(pb_decode(&cmd_stream, keyval_Command_fields, cmd), cmd_stream.errmsg);

    keyval_table kv(_self, _self.value);
    auto kvitr = kv.find(cmd->key);
    if( kvitr == kv.end() ) {
      switch(cmd->which_cmd)
        {
        case keyval_Command_cmdset_tag:
          kv.emplace(worker, [&]( auto& row ) {
                               row.key = cmd->key;
                               row.val = cmd->cmd.cmdset.val;
                             });
          break;

        case keyval_Command_cmddel_tag:
          check(false, "Entry does not exist");
          break;
        default:
          check(false, "Invalid command: " + to_string(cmd->which_cmd));
        };
    }
    else {
      switch(cmd->which_cmd)
        {
        case keyval_Command_cmdset_tag:
          kv.modify(*kvitr, worker, [&]( auto& row ) {
                                      row.val = cmd->cmd.cmdset.val;
                                    });
          break;

        case keyval_Command_cmddel_tag:
          kv.erase(kvitr);
          break;

        default:
          check(false, "Invalid command: " + to_string(cmd->which_cmd));
        };
    }
  }

 private:

  // key-value pairs; scope=_self
  struct [[eosio::table("keyval")]] keyval_row {
    uint32_t           key;
    string             val;
    uint64_t primary_key()const { return key; }
  };

  typedef eosio::multi_index<name("keyval"), keyval_row> keyval_table;


  struct [[eosio::table("iprops")]] iprop {
    name           key;
    uint64_t       val;
    uint64_t primary_key()const { return key.value; }
  };

  typedef eosio::multi_index<name("iprops"), iprop> iprops;

  void set_iprop(name key, uint64_t value)
  {
    iprops p(_self, _self.value);
    auto itr = p.find(key.value);
    if( itr != p.end() ) {
      p.modify(*itr, same_payer, [&]( auto& row ) {
                                   row.val = value;
                                 });
    }
    else {
      p.emplace(_self, [&]( auto& row ) {
                         row.key = key;
                         row.val = value;
                       });
    }
  }

  uint64_t get_iprop(name key)
  {
    iprops p(_self, _self.value);
    auto itr = p.find(key.value);
    if( itr != p.end() ) {
      return itr->val;
    }
    else {
      p.emplace(_self, [&]( auto& row ) {
                         row.key = key;
                         row.val = 0;
                       });
      return 0;
    }
  }

};
