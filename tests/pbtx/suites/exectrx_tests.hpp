#include "pbtx_tester.hpp"

BOOST_AUTO_TEST_SUITE(exectrx_tests)

BOOST_FIXTURE_TEST_CASE(asserts_exectrx_test, pbtx_tester)
try
{
    BOOST_REQUIRE_EQUAL(error("missing authority of bob"), 
                        m_pbtx_api.exectrx(N(alice), N(bob), {}));

    // BOOST_REQUIRE_EQUAL(wasm_assert_msg("Signature too short. Expected 65 bytes or more, got "),
    //                     m_pbtx_api.exectrx(N(alice), N(), {}));

    // BOOST_REQUIRE_EQUAL(wasm_assert_msg("Signature #  does not match any keys of actor #"),
    //                     m_pbtx_api.exectrx(N(alice), N(), {}));

    // BOOST_REQUIRE_EQUAL(wasm_assert_msg("Insufficient signatures weight for actor #"),
    //                     m_pbtx_api.exectrx(N(alice), N(), {}));
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
    auto encoded_trx_body1 = encode_transaction_body(666, string_to_uint64_t("alice"), 0, {}, 1, 0, 0, {33});
    auto alice_sig1 = to_signature(encoded_trx_body1, get_private_key(N(alice), "active"));
    signature signatures1{std::make_tuple(alice_sig1, pbtx_KeyType_EOSIO_KEY, alice_sig1.size())};
    auto trx1 = encode_transaction(encoded_trx_body1, 0, signatures1);
    BOOST_REQUIRE_EQUAL(wasm_assert_msg("Unknown network_id: 666"),
                        m_pbtx_api.exectrx(N(bob), N(bob), trx1));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(unknow_actor_exectrx_test, pbtx_tester)
try
{
    auto encoded_trx_body1 = encode_transaction_body(1001, string_to_uint64_t("carol"), 0, {}, 1, 0, 0, {33});
    auto alice_sig1 = to_signature(encoded_trx_body1, get_private_key(N(alice), "active"));
    signature signatures1{std::make_tuple(alice_sig1, pbtx_KeyType_EOSIO_KEY, alice_sig1.size())};
    auto trx1 = encode_transaction(encoded_trx_body1, 0, signatures1);
    BOOST_REQUIRE_EQUAL(wasm_assert_msg("Unknown actor: 4733081447982694400"),
                        m_pbtx_api.exectrx(N(bob), N(bob), trx1));
}
FC_LOG_AND_RETHROW()

// BOOST_FIXTURE_TEST_CASE(expected_seq_num_exectrx_test, pbtx_tester)
// try
// {
//     auto encoded_trx_body1 = encode_transaction_body(666, string_to_uint64_t("carol"), 0, {}, 1, 0, 0, {33});
//     auto alice_sig1 = to_signature(encoded_trx_body1, get_private_key(N(alice), "active"));
//     signature signatures1{std::make_tuple(alice_sig1, pbtx_KeyType_EOSIO_KEY, alice_sig1.size())};
//     auto trx1 = encode_transaction(encoded_trx_body1, 0, signatures1);
// BOOST_REQUIRE_EQUAL(wasm_assert_msg("Expected seqnum="),
//                     m_pbtx_api.exectrx(N(alice), N(), {}));
// }
// FC_LOG_AND_RETHROW()

// BOOST_FIXTURE_TEST_CASE(prev_body_hash_exectrx_test, pbtx_tester)
// try
// {
// BOOST_REQUIRE_EQUAL(wasm_assert_msg("Previous body hash mismatch. Expected "),
    //                     m_pbtx_api.exectrx(N(alice), N(), {}));
// }
// FC_LOG_AND_RETHROW()

// BOOST_FIXTURE_TEST_CASE(unknown_cosignor_exectrx_test, pbtx_tester)
// try
// {
// }
// FC_LOG_AND_RETHROW()

// BOOST_FIXTURE_TEST_CASE(unknown_cosignor_exectrx_test, pbtx_tester)
// try
// {
// BOOST_REQUIRE_EQUAL(wasm_assert_msg("Unknown cosignor #"),
//                     m_pbtx_api.exectrx(N(alice), N(), {}));
// }
// FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(expected_sig_num_exectrx_test, pbtx_tester)
try
{
    auto encoded_trx_body = encode_transaction_body(1001, string_to_uint64_t("alice"), 0, {}, 1, 0, 0, {33});
    auto alice_sig = to_signature(encoded_trx_body, get_private_key(N(alice), "active"));
    signature signatures{std::make_tuple(alice_sig, pbtx_KeyType_EOSIO_KEY, alice_sig.size())};
    auto trx = encode_transaction(encoded_trx_body, 0, signatures);
    BOOST_REQUIRE_EQUAL(wasm_assert_msg("Expected 1 signatures, but received 0"),
                        m_pbtx_api.exectrx(N(bob), N(bob), trx));

}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(exectrx_test, pbtx_tester)
try
{
    auto encoded_trx_body = encode_transaction_body(1001, string_to_uint64_t("alice"), 0, {}, 1, 0, 0, {33});
    auto alice_sig = to_signature(encoded_trx_body, get_private_key(N(alice), "active"));
    signature signatures{std::make_tuple(alice_sig, pbtx_KeyType_EOSIO_KEY, alice_sig.size())};
    auto trx = encode_transaction(encoded_trx_body, 1, signatures);
    BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.exectrx(N(bob), N(bob), trx));
}
FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
