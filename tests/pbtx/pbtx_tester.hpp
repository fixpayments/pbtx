#pragma once
#include "contracts.hpp"
#include "pbtx_api.hpp"
#include "pbtx.pb.h"
#include "pbtx_contract_constants.hpp"
#include <pb_encode.h>
#include <pb_decode.h>
#include <boost/test/unit_test.hpp>
#include <eosio/testing/tester.hpp>

using namespace eosio::testing;
using namespace eosio;
using namespace eosio::chain;
using namespace fc;
using namespace std;

using mvo = fc::mutable_variant_object;
using key = std::vector<std::tuple<std::vector<char>, pbtx_KeyType, uint32_t>>;
using signature = std::vector<std::vector<std::tuple<std::vector<char>, pbtx_KeyType, pb_size_t>>>;

class pbtx_tester : public tester {
protected:
	pbtx_api m_pbtx_api;

public:
	pbtx_tester();

	std::vector<uint8_t> encode_permisson(const uint64_t &actor, const uint32_t &threshold,
										  const pb_size_t &keys_count, const key &keys);

	std::vector<uint8_t> encode_signature(const pbtx_KeyType &type, const pb_size_t &sigs_count, const std::vector<char> &sigs);

	std::vector<uint8_t> encode_transaction_body(const uint64_t &network_id, const uint64_t &actor,
												 const pb_size_t &cosignors_count, const std::vector<uint64_t> &cosignors,
												 const uint32_t &seqnum, const uint64_t &prev_hash, const uint32_t &transaction_type,
												 const std::vector<char> &transaction_content);

	std::tuple<bool, std::vector<uint8_t>> encode_transaction(const std::vector<uint8_t>  &encoded_trx_body, const pb_size_t &authorities_count, const signature &authorities);

	void decode_permisson(const std::vector<uint8_t> &buffer);
	void decode_signature(const std::vector<uint8_t> &buffer);
	void decode_transaction_body(const std::vector<uint8_t> &buffer);
	void decode_transaction(const std::vector<uint8_t> &buffer);

	std::vector<char> to_signature(const std::vector<uint8_t> &trx_body,
								   const fc::crypto::private_key &prv_key);

private:
	void pbtx_init();
};
