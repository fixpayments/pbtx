#include "pbtx_tester.hpp"

pbtx_tester::pbtx_tester()
	: m_pbtx_api(N(pbtx), this)
{
	produce_blocks(2);

    create_accounts({
        N(alice),
        N(bob),
        N(carol)
    });

    pbtx_init();
}

void pbtx_tester::pbtx_init()
{
    //Predefined networks
    BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.regnetwork(N(bob), 1000, N(bob), {}, 0xFFFF0001));

    BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.regnetwork(N(bob), 1001, N(bob), {}, 0xFFFF0001));

    key keys{{get_public_key(N(alice), "active"), pbtx_KeyType_EOSIO_KEY, 1}};
    auto permisson = encode_permisson(string_to_name("alice").to_uint64_t(), 1, keys.size(), keys);

    // BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.regactor(N(bob), 1001, permisson));
}

std::vector<uint8_t> pbtx_tester::encode_permisson(const uint64_t &actor,
                                                   const uint32_t &threshold,
                                                   const pb_size_t &keys_count,
                                                   const key &keys)
{
    _pbtx_Permission permisson = pbtx_Permission_init_default;

    permisson.actor = actor;
    permisson.threshold = threshold;
    permisson.keys_count = keys_count;

    auto i = 0;

    for (auto [key, key_type, weight] : keys)
    {
        permisson.keys[i].has_key = true;
        permisson.keys[i].key.type = key_type;
        permisson.keys[i].key.key_bytes.size = 32;
        memcpy(permisson.keys[i].key.key_bytes.bytes, &key, 32);
        permisson.keys->weight = weight;
        i++;
    }
    
    auto max_size = 128;
    uint8_t buffer[max_size];
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    auto status = pb_encode(&stream, pbtx_Permission_fields, &permisson);

    if (!status)
    {
        printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
        exit(EXIT_FAILURE);
    }

    return std::vector<uint8_t>{&buffer[0], &buffer[max_size]};
}

std::vector<uint8_t> pbtx_tester::encode_transaction()
{

    return std::vector<uint8_t>();
}


