#include "pbtx.hpp"
#include <eosio/crypto.hpp>
#include "pbtx.pb.h"
#include <pb_decode.h>



ACTION pbtx::regnetwork(uint64_t network_id, name admin_acc, vector<name> listeners)
{
  require_auth(admin_acc);
  networks _networks(_self, 0);
  auto nwitr = _networks.find(network_id);
  if( nwitr == _networks.end() ) {
    for(name rcpt: listeners) {
      require_recipient(rcpt);
    }
    _networks.emplace(admin_acc, [&]( auto& row ) {
                                row.admin_acc = admin_acc;
                                row.listeners = listeners;
                              });
  }
  else {
    // nodeos make sure every recipient gets only one notification
    for(name rcpt: nwitr->listeners) {
      require_recipient(rcpt);
    }
    for(name rcpt: listeners) {
      require_recipient(rcpt);
    }
    _networks.modify(*nwitr, admin_acc, [&]( auto& row ) {
                                            row.listeners = listeners;
                                          });
  }
}



ACTION pbtx::unregnetwrok(uint64_t network_id)
{
  networks _networks(_self, 0);
  auto nwitr = _networks.find(network_id);
  check(nwitr != _networks.end(), "Unknown network");
  require_auth(nwitr->admin_acc);

  actors _actors(_self, network_id);
  check(_actors.begin() == _actors.end(), "Cannot delete a network while it has actors");

  for(name rcpt: nwitr->listeners) {
    require_recipient(rcpt);
  }

  _networks.erase(nwitr);
}




ACTION pbtx::regactor(uint64_t network_id, vector<uint8_t> permission)
{
  networks _networks(_self, 0);
  auto nwitr = _networks.find(network_id);
  check(nwitr != _networks.end(), "Unknown network");
  require_auth(nwitr->admin_acc);

  pbtx_Permission perm;
  pb_istream_t perm_stream = pb_istream_from_buffer(permission.data(), permission.size());
  check(pb_decode(&perm_stream, pbtx_Permission_fields, &perm), perm_stream.errmsg);

  check(perm.threshold > 0, "Threshold cannot be zero");
  uint64_t weights_sum = 0;
  for( uint32_t i = 0; i < perm.keys_count; i++ ) {
    check(perm.keys[i].weight > 0, "Key weight cannot be zero in key #" + to_string(i));
    check(perm.keys[i].key.type == pbtx_KeyType_EOSIO_KEY, "Unknown key type: " + to_string(perm.keys[i].key.type) +
          " in key #" + to_string(i));
    check(perm.keys[i].key.key_bytes.size >= 34, "Key #" + to_string(i) + " is too short");
    weights_sum += perm.keys[i].weight;
  }
  check(weights_sum >= perm.threshold, "Threshold cannot be higher than sum of weights");

  actors _actors(_self, network_id);
  auto actitr = _actors.find(perm.actor);
  if( actitr == _actors.end() ) {
    _actors.emplace(nwitr->admin_acc, [&]( auto& row ) {
        row.actor = perm.actor;
        row.seqnum = 0;
        row.permission = permission;
      });
  }
  else {
    _actors.modify(*actitr, nwitr->admin_acc, [&]( auto& row ) {
        row.permission = permission;
      });
  }

  for(name rcpt: nwitr->listeners) {
    require_recipient(rcpt);
  }
}



ACTION pbtx::unregactor(uint64_t network_id, uint64_t actor)
{
  networks _networks(_self, 0);
  auto nwitr = _networks.find(network_id);
  check(nwitr != _networks.end(), "Unknown network");
  require_auth(nwitr->admin_acc);

  actors _actors(_self, network_id);
  auto actitr = _actors.find(actor);
  check(actitr != _actors.end(), "Unknown actor");

  for(name rcpt: nwitr->listeners) {
    require_recipient(rcpt);
  }

  _actors.erase(actitr);
}



void validate_signature(const checksum256& digest, const vector<uint8_t>& pbperm, const pbtx_Signature& sig)
{
  pbtx_Permission perm;
  pb_istream_t perm_stream = pb_istream_from_buffer(pbperm.data(), pbperm.size());
  check(pb_decode(&perm_stream, pbtx_Permission_fields, &perm), perm_stream.errmsg);

  uint32_t sum_weights = 0;
  
  for( uint32_t i=0; i < sig.sig_bytes_count; i++ ) {
    public_key recovered_key;
    {
      datastream ds(sig.sig_bytes[i].bytes, sig.sig_bytes[i].size);
      signature eosiosig;
      ds >> eosiosig;
      recovered_key = recover_key(digest, eosiosig );
    }

    bool matched = false;
    for( uint32_t j=0; j<perm.keys_count; j++ ) {
      datastream ds(perm.keys[j].key.key_bytes.bytes, perm.keys[j].key.key_bytes.size);
      public_key perm_key;
      ds >> perm_key;
      if( recovered_key == perm_key ) {
        sum_weights += perm.keys[j].weight;
        matched = true;
        break;
      }
    }

    if( !matched ) {
      check(false, "Signature #" + to_string(i) + " does not match any keys of actor #" + to_string(perm.actor));
    }
  }

  if( sum_weights < perm.threshold ) {
    check(false, "Insufficient signatures weight for actor #" + to_string(perm.actor));
  }
}



ACTION pbtx::exectrx(vector<uint8_t> trx_input)
{
  pbtx_Transaction trx;
  pb_istream_t trx_stream = pb_istream_from_buffer(trx_input.data(), trx_input.size());
  check(pb_decode(&trx_stream, pbtx_Transaction_fields, &trx), trx_stream.errmsg);
  check(trx.transaction_content.size > 0, "Empty transaction");
  
  networks _networks(_self, 0);
  auto nwitr = _networks.find(trx.network_id);
  if( nwitr == _networks.end() ) {
    check(false, "Unknown network_id: " + to_string(trx.network_id));
  }
  
  actors _actors(_self, trx.network_id);
  auto actitr = _actors.find(trx.actor);
  if( actitr == _actors.end() ) {
    check(false, "Unknown actor: " + to_string(trx.actor));
  }

  if( trx.seqnum != actitr->seqnum + 1 ) {
    check(false, "Expected seqnum=" + to_string(actitr->seqnum + 1) +
          ", received seqnum=" + to_string(trx.seqnum));
  }

  _actors.modify(*actitr, same_payer, [&]( auto& row ) {
                                        row.seqnum++;
                                      });
  
  if( trx.signatures_count != trx.cosignors_count + 1 ) {
    check(false, "Expected " + to_string(trx.cosignors_count + 1) + " signatures, but received " +
          to_string(trx.signatures_count));
  }

  checksum256 digest = sha256((const char*)trx.transaction_content.bytes, trx.transaction_content.size);

  validate_signature(digest, actitr->permission, trx.signatures[0]);
  for( uint32_t i = 0; i < trx.cosignors_count; i++ ) {
    actitr = _actors.find(trx.cosignors[i]);
    if( actitr == _actors.end() ) {
      check(false, "Unknown cosignor #" + to_string(i) + ": " + to_string(trx.cosignors[i]));
    }
    validate_signature(digest, actitr->permission, trx.signatures[i+1]);
  }

  
  pbtxtransact_abi args =
    {
     trx.actor, trx.seqnum, {trx.cosignors, trx.cosignors + trx.cosignors_count},
     {trx.transaction_content.bytes, trx.transaction_content.bytes + trx.transaction_content.size}
    };
  
  for(name rcpt: nwitr->listeners) {
    action {permission_level{_self, name("active")}, rcpt, name("pbtxtransact"), args}.send();
  }
}
