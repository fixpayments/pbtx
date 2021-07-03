#include "pbtx_tester.hpp"

pbtx_tester::pbtx_tester()
    : m_pbtx_api(N(pbtx), this)
{
    produce_blocks(2);

    create_accounts({N(alice),
                     N(bob),
                     N(carol),
                     N(msiguser1),
                     N(msiguser2),
                     N(msiguser3)});

    pbtx_init();
}

void pbtx_tester::pbtx_init()
{
    //Predefined networks
    BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.regnetwork(N(bob), 998, N(bob), {}, 0xFFFF0002));

    BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.regnetwork(N(bob), 999, N(bob), {}, 0xFFFF0002));

    key keys1{{fc::raw::pack(get_public_key(N(alice), "active")), pbtx_KeyType_EOSIO_KEY, 1}};
    auto permisson1 = encode_permisson(string_to_uint64_t("alice"), 1, keys1.size(), keys1);
    BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.regactor(N(bob), 999, permisson1));

    BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.regnetwork(N(bob), 1000, N(bob), {}, 0xFFFF0001));

    BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.regnetwork(N(bob), 1001, N(bob), {}, 0xFFFF0001));

    key keys2{{fc::raw::pack(get_public_key(N(alice), "active")), pbtx_KeyType_EOSIO_KEY, 1}};
    auto permisson2 = encode_permisson(string_to_uint64_t("alice"), 1, keys2.size(), keys2);
    BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.regactor(N(bob), 1001, permisson2));

    //Multisig network
    BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.regnetwork(N(bob), 2001, N(bob), {}, 0xFFFF0001));

    key keys3{{fc::raw::pack(get_public_key(N(msiguser1), "active")), pbtx_KeyType_EOSIO_KEY, 1},
              {fc::raw::pack(get_public_key(N(msiguser2), "active")), pbtx_KeyType_EOSIO_KEY, 1},
              {fc::raw::pack(get_public_key(N(msiguser3), "active")), pbtx_KeyType_EOSIO_KEY, 1}};

    auto permisson3 = encode_permisson(string_to_uint64_t("msiguser1"), 3, keys3.size(), keys3);
    BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.regactor(N(bob), 2001, permisson3));
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

    for (const auto &[key_buff, key_type, weight] : keys)
    {
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
        printf("Permission encoding failed: %s\n", PB_GET_ERROR(&stream));
        exit(EXIT_FAILURE);
    }

    return buffer;
}

std::vector<uint8_t> pbtx_tester::encode_signature(const pbtx_KeyType &type, const pb_size_t &sig_bytes_count, const std::vector<char> &sig_bytes)
{
    _pbtx_Signature signature = pbtx_Signature_init_default;

    signature.type = type;
    signature.sig_bytes_count = sig_bytes_count;
    signature.sig_bytes->size = sig_bytes.size();
    memcpy(signature.sig_bytes->bytes, &sig_bytes[0], sig_bytes.size());

    std::vector<uint8_t> buffer(pbtx_Signature_size);
    pb_ostream_t stream = pb_ostream_from_buffer(buffer.data(), buffer.size());
    auto status = pb_encode(&stream, pbtx_Signature_fields, &signature);
    buffer.resize(stream.bytes_written);

    if (!status)
    {
        printf("Signature encoding failed: %s\n", PB_GET_ERROR(&stream));
        exit(EXIT_FAILURE);
    }

    return buffer;
}

std::vector<uint8_t> pbtx_tester::encode_transaction_body(const uint64_t &network_id, const uint64_t &actor,
                                                          const pb_size_t &cosignors_count, const std::vector<uint64_t> &cosignors,
                                                          const uint32_t &seqnum, const uint64_t &prev_hash, const uint32_t &transaction_type,
                                                          const std::vector<char> &transaction_content)
{
    _pbtx_TransactionBody trx_body = pbtx_TransactionBody_init_default;

    trx_body.network_id = network_id;
    trx_body.actor = actor;
    trx_body.cosignors_count = cosignors_count;
    memcpy(trx_body.cosignors, &cosignors[0], cosignors.size());
    trx_body.seqnum = seqnum;
    trx_body.prev_hash = prev_hash;
    trx_body.transaction_type = transaction_type;
    trx_body.transaction_content.size = transaction_content.size();
    memcpy(trx_body.transaction_content.bytes, &transaction_content[0], transaction_content.size());

    std::vector<uint8_t> buffer(pbtx_TransactionBody_size);
    pb_ostream_t stream = pb_ostream_from_buffer(buffer.data(), buffer.size());
    auto status = pb_encode(&stream, pbtx_TransactionBody_fields, &trx_body);
    buffer.resize(stream.bytes_written);

    if (!status)
    {
        printf("Transaction body encoding failed: %s\n", PB_GET_ERROR(&stream));
        exit(EXIT_FAILURE);
    }

    return buffer;
}

std::tuple<bool, std::vector<uint8_t>> pbtx_tester::encode_transaction(const std::vector<uint8_t> &encoded_trx_body, const pb_size_t &signatures_count, const signature &signatures)
{
    _pbtx_Transaction transaction = pbtx_Transaction_init_default;

    transaction.body.size = encoded_trx_body.size();
    memcpy(transaction.body.bytes, &encoded_trx_body[0], encoded_trx_body.size());
    transaction.signatures_count = signatures_count;

    auto i = 0;
    for (const auto &signature : signatures)
    {
        auto [sig, type, sig_bytes_count] = signature[i];
        transaction.signatures[i].type = type;
        transaction.signatures[i].sig_bytes_count = signature.size();

        auto j = 0;
        for (const auto &ecc_sig : signature)
        {
            auto bytes = std::get<0>(ecc_sig);
            transaction.signatures[i].sig_bytes[j].size = bytes.size();
            memcpy(transaction.signatures[i].sig_bytes[j].bytes , &bytes[0], bytes.size());
            j++;
        }

        i++;
    }

    std::vector<uint8_t> buffer(pbtx_Transaction_size);
    pb_ostream_t stream = pb_ostream_from_buffer(buffer.data(), buffer.size());
    auto status = pb_encode(&stream, pbtx_Transaction_fields, &transaction);
    buffer.resize(stream.bytes_written);

    if (!status)
    {
        printf("Transaction encoding failed: %s\n", PB_GET_ERROR(&stream));
        // exit(EXIT_FAILURE);
    }

    return std::make_tuple(status,buffer);
}

void pbtx_tester::decode_permisson(const std::vector<uint8_t> &buffer)
{
    _pbtx_Permission permisson = pbtx_Permission_init_default;

    pb_istream_t stream = pb_istream_from_buffer(buffer.data(), buffer.size());

    auto status = pb_decode(&stream, pbtx_Permission_fields, &permisson);

    if (!status)
    {
        printf("Decoding permisson failed: %s\n", PB_GET_ERROR(&stream));
        exit(EXIT_FAILURE);
    }
    
    printf("Actor: %lu\n", permisson.actor);
    printf("Threshold: %u\n", permisson.threshold);
    printf("Keys count: %u\n", permisson.keys_count);

    for (auto i = 0; i < permisson.keys_count; i++)
    {
        printf("Has key: %d\n", permisson.keys[0].has_key);
        printf("Key type: %u\n", permisson.keys[0].key.type);
        printf("Key weight: %u\n", permisson.keys[0].weight);
        printf("Key size: %u\n", permisson.keys[0].key.key_bytes.size);

        printf("Key bytes: ");
        for(auto j = 0; j < permisson.keys[0].key.key_bytes.size; j++)
        {
            printf("%02hhx", permisson.keys[0].key.key_bytes.bytes[j]);
        }
    }
}

void pbtx_tester::decode_signature(const std::vector<uint8_t> &buffer)
{
    _pbtx_Signature signature = pbtx_Signature_init_default;

    pb_istream_t stream = pb_istream_from_buffer(buffer.data(), buffer.size());

    auto status = pb_decode(&stream, pbtx_Signature_fields, &signature);

    if (!status)
    {
        printf("Decoding signature failed: %s\n", PB_GET_ERROR(&stream));
        exit(EXIT_FAILURE);
    }
}
	
void pbtx_tester::decode_transaction_body(const std::vector<uint8_t> &buffer)
{
    _pbtx_TransactionBody trx_body = pbtx_TransactionBody_init_default;

    pb_istream_t stream = pb_istream_from_buffer(buffer.data(), buffer.size());

    auto status = pb_decode(&stream, pbtx_TransactionBody_fields, &trx_body);

    if (!status)
    {
        printf("Decoding transaction body failed: %s\n", PB_GET_ERROR(&stream));
        exit(EXIT_FAILURE);
    }
}

void pbtx_tester::decode_transaction(const std::vector<uint8_t> &buffer)
{
    _pbtx_Transaction transaction = pbtx_Transaction_init_default;

    pb_istream_t stream = pb_istream_from_buffer(buffer.data(), buffer.size());

    auto status = pb_decode(&stream, pbtx_Transaction_fields, &transaction);

    if (!status)
    {
        printf("Decoding transaction failed: %s\n", PB_GET_ERROR(&stream));
        exit(EXIT_FAILURE);
    }

    printf("Signatures count: %u\n", transaction.signatures_count);

    for (auto i = 0; i < transaction.signatures_count; i++)
    {
        printf("Signatures type: %u\n", transaction.signatures[i].type);
        printf("Signatures byte buffers count: %u\n", transaction.signatures[i].sig_bytes_count);

        for(auto j = 0; j < transaction.signatures[i].sig_bytes_count; j++)
        {
            printf("Signatures byte buffer size: %u\n", transaction.signatures[i].sig_bytes[j].size);

            printf("Signatures byte buffer bytes: ");
            for(auto k = 0; k < transaction.signatures[i].sig_bytes[j].size; k++)
            {
                printf("%02hhx", transaction.signatures[i].sig_bytes[j].bytes[k]);
            }
            printf("\n");
        }
       
    }
}

std::vector<char> pbtx_tester::to_signature(const std::vector<uint8_t> &trx_body,
                                            const fc::crypto::private_key &prv_key)
{
    auto sha256 = fc::sha256::hash((const char*)trx_body.data(), trx_body.size());
    auto signature = prv_key.sign(sha256, false);
    return fc::raw::pack(signature);
}