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

    auto permisson = encode_permisson();

    BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.regactor(N(bob), 1001, permisson));
}

std::vector<uint8_t> pbtx_tester::encode_permisson()
{
    _pbtx_Permission permisson = pbtx_Permission_init_default;
    auto alice_pub_key = get_public_key(N(alice), "active").to_string();

    permisson.actor = string_to_name("alice").to_uint64_t();
    permisson.threshold = 1;
    permisson.keys_count = 1;
    permisson.keys->has_key = true;
    permisson.keys->key.type = pbtx_KeyType_EOSIO_KEY;
    permisson.keys->key.key_bytes.size = alice_pub_key.size();
    // permisson.keys->key.key_bytes.bytes = alice_pub_key.c_str();
    permisson.keys->weight = 1;

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


