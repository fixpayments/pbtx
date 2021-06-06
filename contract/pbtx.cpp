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
    std::set<name> recipients;
    for(name rcpt: nwitr->listeners) {
      recipients.insert(rcpt);
    }
    for(name rcpt: listeners) {
      recipients.insert(rcpt);
    }
    for(name rcpt: recipients) {
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


ACTION pbtx::regactor(uint64_t network_id, vector<uint8_t> permisison)
{
  networks _networks(_self, 0);
  auto nwitr = _networks.find(network_id);
  check(nwitr != _networks.end(), "Unknown network");
  require_auth(nwitr->admin_acc);

  
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



uint8_t* trx_content_pointer = NULL;
size_t trx_content_len = 0;

bool pbtx_Transaction_decode_content(pb_istream_t *istream, const pb_field_t *field, void **arg)
{
  trx_content_len = istream->bytes_left;
  trx_content_pointer = (uint8_t*) malloc(trx_content_len);
  if (!pb_read(istream, trx_content_pointer, trx_content_len)) {
    trx_content_len = 0;
    return false;
  }
  return true;
}



ACTION pbtx::exectrx(vector<uint8_t> trx_input)
{
  pbtx_Transaction trx;
  trx.transaction_content.funcs.decode = pbtx_Transaction_decode_content;
    
  pb_istream_t trx_stream = pb_istream_from_buffer(trx_input.data(), trx_input.size());
  check(pb_decode(&trx_stream, pbtx_Transaction_fields, &trx), trx_stream.errmsg);

  check(trx_content_len > 0, "empty content");

}

