#include "pbtx_tester.hpp"

BOOST_AUTO_TEST_SUITE(exectrx_tests)

BOOST_FIXTURE_TEST_CASE(authority_exectrx_test, pbtx_tester)
try
{
    BOOST_REQUIRE_EQUAL(error("missing authority of bob"), 
                        m_pbtx_api.exectrx(N(alice), N(bob), {}));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(empty_trx_body_exectrx_test, pbtx_tester)
try
{
   BOOST_REQUIRE_EQUAL(wasm_assert_msg("Empty transaction body"),
                        m_pbtx_api.exectrx(N(bob), N(bob), {}));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(unknow_network_exectrx_test, pbtx_tester)
try
{
    auto encoded_trx_body = encode_transaction_body(666, string_to_uint64_t("alice"), 0, {}, 1, 0, 0, {33});
    auto alice_sig = to_signature(encoded_trx_body, get_private_key(N(alice), "active"));
    signature signatures{{std::make_tuple(alice_sig, pbtx_KeyType_EOSIO_KEY, alice_sig.size())}};
    auto [status, trx] = encode_transaction(encoded_trx_body, 0, signatures);
    BOOST_REQUIRE_EQUAL(true, status);
    BOOST_REQUIRE_EQUAL(wasm_assert_msg("Unknown network_id: 666"),
                        m_pbtx_api.exectrx(N(bob), N(bob), trx));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(unknow_actor_exectrx_test, pbtx_tester)
try
{
    auto encoded_trx_body = encode_transaction_body(1001, string_to_uint64_t("carol"), 0, {}, 1, 0, 0, {33});
    auto alice_sig = to_signature(encoded_trx_body, get_private_key(N(alice), "active"));
    signature signatures{{std::make_tuple(alice_sig, pbtx_KeyType_EOSIO_KEY, alice_sig.size())}};
    auto [status, trx] = encode_transaction(encoded_trx_body, 0, signatures);
    BOOST_REQUIRE_EQUAL(true, status);
    BOOST_REQUIRE_EQUAL(wasm_assert_msg("Unknown actor: 4733081447982694400"),
                        m_pbtx_api.exectrx(N(bob), N(bob), trx));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(expected_seq_num_exectrx_test, pbtx_tester)
try
{
    auto encoded_trx_body = encode_transaction_body(1001, string_to_uint64_t("alice"), 0, {}, 0, 0, 0, {33});
    auto alice_sig = to_signature(encoded_trx_body, get_private_key(N(alice), "active"));
    signature signatures{{std::make_tuple(alice_sig, pbtx_KeyType_EOSIO_KEY, alice_sig.size())}};
    auto [status, trx] = encode_transaction(encoded_trx_body, 0, signatures);
    BOOST_REQUIRE_EQUAL(true, status);
    BOOST_REQUIRE_EQUAL(wasm_assert_msg("Expected seqnum=1, received seqnum=0"),
                        m_pbtx_api.exectrx(N(bob), N(bob), trx));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(prev_body_hash_exectrx_test, pbtx_tester)
try
{
    auto encoded_trx_body = encode_transaction_body(1001, string_to_uint64_t("alice"), 0, {}, 1, 1, 0, {33});
    auto alice_sig = to_signature(encoded_trx_body, get_private_key(N(alice), "active"));
    signature signatures{{std::make_tuple(alice_sig, pbtx_KeyType_EOSIO_KEY, alice_sig.size())}};
    auto [status, trx] = encode_transaction(encoded_trx_body, signatures.size(), signatures);
    BOOST_REQUIRE_EQUAL(true, status);
    BOOST_REQUIRE_EQUAL(wasm_assert_msg("Previous body hash mismatch. Expected 0, received prev_hash=1"),
                        m_pbtx_api.exectrx(N(bob), N(bob), trx));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(expected_sig_num_exectrx_test, pbtx_tester)
try
{
    auto encoded_trx_body = encode_transaction_body(1001, string_to_uint64_t("alice"), 0, {}, 1, 0, 0, {33});
    auto alice_sig = to_signature(encoded_trx_body, get_private_key(N(alice), "active"));
    signature signatures{{std::make_tuple(alice_sig, pbtx_KeyType_EOSIO_KEY, alice_sig.size())}};
    auto [status, trx] = encode_transaction(encoded_trx_body, 0, signatures);
    BOOST_REQUIRE_EQUAL(true, status);
    BOOST_REQUIRE_EQUAL(wasm_assert_msg("Expected 1 signatures, but received 0"),
                        m_pbtx_api.exectrx(N(bob), N(bob), trx));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(unknown_cosignor_exectrx_test, pbtx_tester)
try
{
    //@TODO fix
    // auto encoded_trx_body = encode_transaction_body(1001, string_to_uint64_t("alice"), 1, {string_to_uint64_t("carol")}, 1, 0, 0, {33});
    // auto alice_sig = to_signature(encoded_trx_body, get_private_key(N(alice), "active"));
    // auto carol_sig = to_signature(encoded_trx_body, get_private_key(N(carol), "active"));
    // signature signatures{{std::make_tuple(alice_sig, pbtx_KeyType_EOSIO_KEY, alice_sig.size()), std::make_tuple(carol_sig, pbtx_KeyType_EOSIO_KEY, carol_sig.size())}};

    // auto [status, trx] = encode_transaction(encoded_trx_body, 2, signatures);
    // BOOST_REQUIRE_EQUAL(true, status);
    // BOOST_REQUIRE_EQUAL(wasm_assert_msg("Unknown cosignor #0"),
    //                     m_pbtx_api.exectrx(N(bob), N(bob), trx));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(short_sig_exectrx_test, pbtx_tester)
try
{
    auto encoded_trx_body = encode_transaction_body(1001, string_to_uint64_t("alice"), 0, {}, 1, 0, 0, {33});
    auto alice_sig = to_signature(encoded_trx_body, get_private_key(N(alice), "active"));
    std::vector<char> empty_sig;
    signature signatures{{std::make_tuple(empty_sig, pbtx_KeyType_EOSIO_KEY, 1)}};
    auto [status, trx] = encode_transaction(encoded_trx_body, signatures.size(), signatures);
    BOOST_REQUIRE_EQUAL(true, status);
    BOOST_REQUIRE_EQUAL(wasm_assert_msg("Signature too short. Expected 65 bytes or more, got 0"),
                    m_pbtx_api.exectrx(N(bob), N(bob), trx));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(sig_not_match_actor_exectrx_test, pbtx_tester)
try
{
    auto encoded_trx_body = encode_transaction_body(1001, string_to_uint64_t("alice"), 0, {}, 1, 0, 0, {33});
    auto carol_sig = to_signature(encoded_trx_body, get_private_key(N(carol), "active"));
    signature signatures{{std::make_tuple(carol_sig, pbtx_KeyType_EOSIO_KEY, carol_sig.size())}};
    auto [status, trx] = encode_transaction(encoded_trx_body, signatures.size(), signatures);
    BOOST_REQUIRE_EQUAL(true, status);
    // BOOST_REQUIRE_EQUAL(wasm_assert_msg("Signature #0 does not match any keys of actor #" + std::to_string(string_to_uint64_t("alice"))),
    //                     m_pbtx_api.exectrx(N(bob), N(bob), trx));
}
FC_LOG_AND_RETHROW()

// BOOST_FIXTURE_TEST_CASE(insufficient_sig_weight_exectrx_test, pbtx_tester)
// try
// {
// BOOST_REQUIRE_EQUAL(wasm_assert_msg("Insufficient signatures weight for actor #0"),
//                     m_pbtx_api.exectrx(N(alice), N(), {}));
// }
// FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(exectrx_test, pbtx_tester)
try
{
    //@TODO fix
    auto encoded_trx_body = encode_transaction_body(1001, string_to_uint64_t("alice"), 0, {}, 1, 0, 0, {33});

    auto alice_sig = to_signature(encoded_trx_body, get_private_key(N(alice), "active"));


    std::cout << "\n>>>>>>>>>>>>ENCODING<<<<<<<<<<<<<" << std::endl;
    std::cout << "Size:" << alice_sig.size() << std::endl;

    auto m_sha256 = fc::sha256::hash(encoded_trx_body);
    auto m_signature =get_private_key(N(alice), "active").sign(m_sha256, false);

    std::cout << "Public key: " << get_public_key(N(alice), "active").to_string() << std::endl;
    std::cout << "Private key: " << get_private_key(N(alice), "active").to_string() << std::endl;
    std::cout << "Signature : " << m_signature.to_string() << std::endl;
    std::cout << "Recovery key: " << fc::crypto::public_key(m_signature, m_sha256, false).to_string() << std::endl;

    signature signatures{{std::make_tuple(alice_sig, pbtx_KeyType_EOSIO_KEY, alice_sig.size())}};
    std::cout << "Signature Size:" << alice_sig.size() << std::endl;
    std::cout << "Signature : ";
    for (auto& el : alice_sig)
        printf("%02hhx", el);

    std::cout << "\n>>>>>>>>>>>>DECODING<<<<<<<<<<<<<" << std::endl;

    auto [status,trx] = encode_transaction(encoded_trx_body, signatures.size(), signatures);

    BOOST_REQUIRE_EQUAL(true, status);

    std::cout << "Encoded trx(hex): ";
    for (auto& el : trx)
        printf("%02hhx", el);
    std::cout << std::endl;


    decode_transaction(trx);

    auto actor_permission = m_pbtx_api.get_actor_permission(1001, string_to_uint64_t("alice"));
    std::vector<uint8_t> vec;
    from_variant(actor_permission["permission"].get_array(), vec);

    std::cout << ">>>Permission<<<" << std::endl;

    decode_permisson(vec);

    auto pub_key = fc::raw::pack(get_public_key(N(alice), "active"));

    std::cout << "\nDecoded public key: ";
    for (auto& el : pub_key)
        printf("%02hhx", el);
    std::cout << std::endl;
    BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.exectrx(N(bob), N(bob), trx));

    //@TODO add check history
}
FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
