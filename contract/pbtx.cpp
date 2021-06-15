#include "pbtx.hpp"
#include <eosio/crypto.hpp>
#include <eosio/system.hpp>
#include "pbtx.pb.h"
#include <pb_decode.h>



ACTION pbtx::regnetwork(uint64_t network_id, name admin_acc, vector<name> listeners, uint32_t flags)
{
  require_auth(admin_acc);

  check(((flags & PBTX_FLAGS_PBTX_RESERVED) & ~PBTX_FLAGS_PBTX_KNOWN) == 0,
        "Unrecognized bits set in lower 16 bits of flags");

  networks _networks(_self, 0);
  auto nwitr = _networks.find(network_id);
  if( nwitr == _networks.end() ) {
    for(name rcpt: listeners) {
      require_recipient(rcpt);
    }
    _networks.emplace(admin_acc, [&]( auto& row ) {
                                   row.network_id = network_id;
                                   row.admin_acc = admin_acc;
                                   row.listeners = listeners;
                                   row.flags = flags;
                                 });
  }
  else {
    if( nwitr->admin_acc != admin_acc ) {
      require_auth(nwitr->admin_acc);
    }

    // nodeos makes sure every recipient gets only one notification
    for(name rcpt: nwitr->listeners) {
      require_recipient(rcpt);
    }
    for(name rcpt: listeners) {
      require_recipient(rcpt);
    }
    _networks.modify(*nwitr, admin_acc, [&]( auto& row ) {
                                          row.admin_acc = admin_acc;
                                          row.listeners = listeners;
                                          row.flags = flags;
                                        });
  }
}



ACTION pbtx::unregnetwork(uint64_t network_id)
{
  networks _networks(_self, 0);
  auto nwitr = _networks.find(network_id);
  check(nwitr != _networks.end(), "Unknown network");
  require_auth(nwitr->admin_acc);

  actorperm _actorperm(_self, network_id);
  check(_actorperm.begin() == _actorperm.end(), "Cannot delete a network while it has actors");

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

  actorperm _actorperm(_self, network_id);
  auto actpermitr = _actorperm.find(perm.actor);
  if( actpermitr == _actorperm.end() ) {
    _actorperm.emplace(nwitr->admin_acc, [&]( auto& row ) {
        row.actor = perm.actor;
        row.permission = permission;
      });

    actorseq _actorseq(_self, network_id);
    _actorseq.emplace(nwitr->admin_acc, [&]( auto& row ) {
        row.actor = perm.actor;
        row.seqnum = 0;
      });
  }
  else {
    _actorperm.modify(*actpermitr, nwitr->admin_acc, [&]( auto& row ) {
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

  actorperm _actorperm(_self, network_id);
  auto actpermitr = _actorperm.find(actor);
  check(actpermitr != _actorperm.end(), "Unknown actor");
  _actorperm.erase(actpermitr);

  actorseq _actorseq(_self, network_id);
  auto actseqitr = _actorseq.find(actor);
  check(actseqitr != _actorseq.end(), "Exception 1");
  _actorseq.erase(actseqitr);

  for(name rcpt: nwitr->listeners) {
    require_recipient(rcpt);
  }
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



ACTION pbtx::exectrx(name worker, vector<uint8_t> trx_input)
{
  require_auth(worker);

  pbtx_Transaction trx;
  pb_istream_t trx_stream = pb_istream_from_buffer(trx_input.data(), trx_input.size());
  check(pb_decode(&trx_stream, pbtx_Transaction_fields, &trx), trx_stream.errmsg);
  check(trx.transaction_content.size > 0, "Empty transaction");

  networks _networks(_self, 0);
  auto nwitr = _networks.find(trx.network_id);
  if( nwitr == _networks.end() ) {
    check(false, "Unknown network_id: " + to_string(trx.network_id));
  }

  actorperm _actorperm(_self, trx.network_id);
  auto actpermitr = _actorperm.find(trx.actor);
  if( actpermitr == _actorperm.end() ) {
    check(false, "Unknown actor: " + to_string(trx.actor));
  }

  actorseq _actorseq(_self, trx.network_id);
  auto actseqitr = _actorseq.find(trx.actor);
  check(actseqitr != _actorseq.end(), "Exception 2");

  if( trx.seqnum != actseqitr->seqnum + 1 ) {
    check(false, "Expected seqnum=" + to_string(actseqitr->seqnum + 1) +
          ", received seqnum=" + to_string(trx.seqnum));
  }

  _actorseq.modify(*actseqitr, same_payer, [&]( auto& row ) {
                                             row.seqnum++;
                                             row.last_modified = current_time_point();
                                           });

  if( trx.signatures_count != trx.cosignors_count + 1 ) {
    check(false, "Expected " + to_string(trx.cosignors_count + 1) + " signatures, but received " +
          to_string(trx.signatures_count));
  }

  checksum256 digest = sha256((const char*)trx.transaction_content.bytes, trx.transaction_content.size);

  validate_signature(digest, actpermitr->permission, trx.signatures[0]);
  for( uint32_t i = 0; i < trx.cosignors_count; i++ ) {
    actpermitr = _actorperm.find(trx.cosignors[i]);
    if( actpermitr == _actorperm.end() ) {
      check(false, "Unknown cosignor #" + to_string(i) + ": " + to_string(trx.cosignors[i]));
    }
    validate_signature(digest, actpermitr->permission, trx.signatures[i+1]);
  }


  if( nwitr->flags & PBTX_FLAG_RAW_NOTIFY ) {
    for(name rcpt: nwitr->listeners) {
      require_recipient(rcpt);
    }
  }
  else {
    pbtxtransact_abi args =
      {
       worker, trx.actor, trx.seqnum, {trx.cosignors, trx.cosignors + trx.cosignors_count},
       {trx.transaction_content.bytes, trx.transaction_content.bytes + trx.transaction_content.size}
      };

    vector<permission_level> perms{permission_level{_self, name("active")},
                                   permission_level{worker, name("active")}};

    for(name rcpt: nwitr->listeners) {
      action {perms, rcpt, name("pbtxtransact"), args}.send();
    }
  }
}
