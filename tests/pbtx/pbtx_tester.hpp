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

class pbtx_tester : public tester {
protected:
	pbtx_api m_pbtx_api;

public:
	pbtx_tester();

private:
	void pbtx_init();

	std::vector<uint8_t> encode_permisson();
	std::vector<uint8_t> encode_transaction();
};
