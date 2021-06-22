#include "pbtx_tester.hpp"

pbtx_tester::pbtx_tester()
    : m_pbtx_api(N(pbtx), this)
{
    produce_blocks(2);

    create_accounts({N(alice),
                     N(bob),
                     N(carol)});

    pbtx_init();
}

void pbtx_tester::pbtx_init()
{
    //Predefined networks
    BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.regnetwork(N(bob), 1000, N(bob), {}, 0xFFFF0001));

    BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.regnetwork(N(bob), 1001, N(bob), {}, 0xFFFF0001));

    // key keys{{get_public_key(N(alice), "active"), pbtx_KeyType_EOSIO_KEY, 1}};
    // auto permisson = encode_permisson(string_to_uint64_t("alice"), 1, keys.size(), keys);

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

    for (const auto &[key, key_type, weight] : keys)
    {
        std::vector<char> key_buff = fc::raw::pack(key);

        permisson.keys[i].has_key = true;
        permisson.keys[i].key.type = key_type;
        permisson.keys[i].weight = weight;
        permisson.keys[i].key.key_bytes.size = key_buff.size();
        memcpy(permisson.keys[i].key.key_bytes.bytes, &key_buff[0], key_buff.size());

        i++;
    }

    std::vector<uint8_t> buffer(pbtx_Permission_size);
    pb_ostream_t stream = pb_ostream_from_buffer(buffer.data(), buffer.size());
    auto status = pb_encode(&stream, pbtx_Permission_fields, &permisson);
    buffer.resize(stream.bytes_written);

    if (!status)
    {
        printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
        exit(EXIT_FAILURE);
    }

    return buffer;
}

std::vector<uint8_t> pbtx_tester::encode_transaction()
{

    return std::vector<uint8_t>();
}

void pbtx_tester::decode_permisson(const std::vector<uint8_t> &buffer)
{
    _pbtx_Permission permisson = pbtx_Permission_init_default;

    pb_istream_t stream = pb_istream_from_buffer(buffer.data(), buffer.size());

    auto status = pb_decode(&stream, pbtx_Permission_fields, &permisson);

    if (!status)
    {
        printf("Decoding failed: %s\n", PB_GET_ERROR(&stream));
        exit(EXIT_FAILURE);
    }

    printf("Actor: %lu\n", permisson.actor);
    printf("Threshold: %u\n", permisson.threshold);
    printf("Keys count: %u\n", permisson.keys_count);

    for (auto i = 0; i <= permisson.keys_count; i++)
    {
        printf("Has key: %d\n", permisson.keys[0].has_key);
        printf("Key type: %u\n", permisson.keys[0].key.type);
        printf("Key weight: %u\n", permisson.keys[0].weight);
        printf("Key size: %u\n", permisson.keys[0].key.key_bytes.size);
        printf("Key bytes: %s\n", permisson.keys[0].key.key_bytes.bytes);
    }
}

void pbtx_tester::decode_transaction(const std::vector<uint8_t> &buffer)
{

}

