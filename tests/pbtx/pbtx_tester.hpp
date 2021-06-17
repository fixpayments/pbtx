#pragma once
#include "contracts.hpp"
#include "pbtx_api.hpp"
#include "pbtx.pb.h"
#include <pb_encode.h>
#include <boost/test/unit_test.hpp>
#include <eosio/testing/tester.hpp>

using namespace eosio::testing;
using namespace eosio;
using namespace eosio::chain;
using namespace fc;
using namespace std;

using mvo = fc::mutable_variant_object;
using key = std::vector<std::tuple<fc::crypto::public_key, pbtx_KeyType, uint32_t>>;

class pbtx_tester : public tester {
protected:
	pbtx_api m_pbtx_api;

public:
	pbtx_tester();

private:
	void pbtx_init();

	std::vector<uint8_t> encode_permisson(const uint64_t &actor,
										  const uint32_t &threshold,
										  const pb_size_t &keys_count,
										  const key &keys);

	std::vector<uint8_t> encode_transaction();
};
