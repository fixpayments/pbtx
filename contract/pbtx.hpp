#include <eosio/eosio.hpp>
#include <eosio/multi_index.hpp>
#include <eosio/time.hpp>

using namespace eosio;
using namespace std;



CONTRACT pbtx : public eosio::contract {
 public:

  pbtx( name self, name code, datastream<const char*> ds ):
    contract(self, code, ds)
    {}

  /*
    Per-network flags. Upper 16 bits are free to use by listener
    contracts for their internal needs. Lowe 16 bits are reserved for
    PBTX own use.
  */
  const uint32_t PBTX_FLAGS_PBTX_RESERVED = 0x0000FFFF;
  const uint32_t PBTX_FLAGS_PBTX_KNOWN = 0x00000001;
  const uint32_t PBTX_FLAG_RAW_NOTIFY = 1<<0;


  ACTION regnetwork(uint64_t network_id, name admin_acc, vector<name> listeners, uint32_t flags);

  ACTION unregnetwrok(uint64_t network_id);

  ACTION regactor(uint64_t network_id, vector<uint8_t> permission);

  ACTION unregactor(uint64_t network_id, uint64_t actor);

  // execute the transaction
  ACTION exectrx(vector<uint8_t> trx_input);

  struct pbtxtransact_abi {
    uint64_t           actor;
    uint32_t           seqnum;
    vector<uint64_t>   cosignors;
    vector<uint8_t>    transaction_content;
  };

 private:

  // registry of networks and their admins
  struct [[eosio::table("networks")]] networks_row {
    uint64_t           network_id;
    name               admin_acc;
    vector<name>       listeners;
    uint32_t           flags;
    auto primary_key()const { return network_id; }
  };

  typedef eosio::multi_index<name("networks"), networks_row> networks;


  // actors registry. Scope=network_id
  struct [[eosio::table("actors")]] actors_row {
    uint64_t           actor;
    uint32_t           seqnum;     // sequence number. Only transactions with seqnum+1 are accepted
    time_point         last_modified;
    vector<uint8_t>    permission; // protobuf encoded Permission message
    auto primary_key()const { return actor; }
  };

  typedef eosio::multi_index<name("actors"), actors_row> actors;
};
