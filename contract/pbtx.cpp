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
    check(perm.keys[i].weight > 0, "Key weight cannot be zero in key #" + std::to_string(i));
    check(perm.keys[i].key.type == pbtx_KeyType_EOSIO_KEY, "Unknown key type: " + std::to_string(perm.keys[i].key.type) +
          " in key #" + std::to_string(i));
    check(perm.keys[i].key.key_bytes.size >= 34, "Key #" + std::to_string(i) + " is too short");
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




ACTION pbtx::exectrx(vector<uint8_t> trx_input)
{
  pbtx_Transaction trx;
  pb_istream_t trx_stream = pb_istream_from_buffer(trx_input.data(), trx_input.size());
  check(pb_decode(&trx_stream, pbtx_Transaction_fields, &trx), trx_stream.errmsg);

  networks _networks(_self, 0);
  auto nwitr = _networks.find(trx.network_id);
  check(nwitr != _networks.end(), "Unknown network");

  actors _actors(_self, trx.network_id);
  auto actitr = _actors.find(trx.actor);
  check(actitr != _actors.end(), "Unknown actor");

  if( trx.seqnum != actitr->seqnum + 1 ) {
    check(false, "Expected seqnum=" + std::to_string(actitr->seqnum + 1) +
          ", received seqnum=" + std::to_string(trx.seqnum));
  }

}
