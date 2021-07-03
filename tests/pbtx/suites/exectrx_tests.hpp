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
    auto encoded_trx_body = encode_transaction_body(1001, string_to_uint64_t("alice"), 1, {string_to_uint64_t("carol")}, 1, 0, 0, {33});
    auto alice_sig = to_signature(encoded_trx_body, get_private_key(N(alice), "active"));
    auto alice2_sig = to_signature(encoded_trx_body, get_private_key(N(alice), "active"));
    signature signatures{{std::make_tuple(alice_sig, pbtx_KeyType_EOSIO_KEY, alice_sig.size()),
                          std::make_tuple(alice2_sig, pbtx_KeyType_EOSIO_KEY, alice2_sig.size())}};

    auto [status, trx] = encode_transaction(encoded_trx_body, signatures.data()->size(), signatures);
    BOOST_REQUIRE_EQUAL(true, status);
    BOOST_REQUIRE_EQUAL(wasm_assert_msg("Unknown cosignor #0: 0"),
                        m_pbtx_api.exectrx(N(bob), N(bob), trx));
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
    BOOST_REQUIRE_EQUAL(wasm_assert_msg("Signature #0 does not match any keys of actor #" + std::to_string(string_to_uint64_t("alice"))),
                        m_pbtx_api.exectrx(N(bob), N(bob), trx));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(insufficient_sig_weight_exectrx_test, pbtx_tester)
try
{
    auto encoded_trx_body = encode_transaction_body(2001, string_to_uint64_t("msiguser1"), 0, {}, 1, 0, 0, {33});
    auto msiguser1_sig = to_signature(encoded_trx_body, get_private_key(N(msiguser1), "active"));
    signature signatures{{std::make_tuple(msiguser1_sig, pbtx_KeyType_EOSIO_KEY, msiguser1_sig.size())}};
    auto [status, trx] = encode_transaction(encoded_trx_body, signatures.size(), signatures);
    BOOST_REQUIRE_EQUAL(true, status);

    BOOST_REQUIRE_EQUAL(wasm_assert_msg("Insufficient signatures weight for actor #" + std::to_string(string_to_uint64_t("msiguser1"))),
                       m_pbtx_api.exectrx(N(bob), N(bob), trx));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(exectrx_test, pbtx_tester)
try
{
    auto encoded_trx_body = encode_transaction_body(999, string_to_uint64_t("alice"), 0, {}, 1, 0, 0, {33});
    auto alice_sig = to_signature(encoded_trx_body, get_private_key(N(alice), "active"));

    signature signatures{{std::make_tuple(alice_sig, pbtx_KeyType_EOSIO_KEY, alice_sig.size())}};

    auto [status,trx] = encode_transaction(encoded_trx_body, signatures.size(), signatures);

    BOOST_REQUIRE_EQUAL(true, status);

    BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.exectrx(N(bob), N(bob), trx));

    auto history_id = m_pbtx_api.get_history_id(999);

    REQUIRE_MATCHING_OBJECT(history_id, mvo()
    ("network_id", 999)
    ("last_history_id", 2));

    auto history = m_pbtx_api.get_history(999, history_id["last_history_id"].as_uint64());

    REQUIRE_MATCHING_OBJECT(history, mvo()
    ("id", history_id["last_history_id"].as_uint64())
    ("event_type", PBTX_HISTORY_EVENT_EXECTRX)
    ("data", trx)
    ("trx_id", "ad0a1877f822ff0e33d8279a169554c5fb2b324e32d2134c4d696b8721a17cd2")
    ("trx_time", "2020-01-01T00:00:08.000"));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(multisig_exectrx_test, pbtx_tester)
try
{
    auto encoded_trx_body = encode_transaction_body(2001, string_to_uint64_t("msiguser1"), 0, {}, 1, 0, 0, {33});
    auto msiguser1_sig = to_signature(encoded_trx_body, get_private_key(N(msiguser1), "active"));
    auto msiguser2_sig = to_signature(encoded_trx_body, get_private_key(N(msiguser2), "active"));
    auto msiguser3_sig = to_signature(encoded_trx_body, get_private_key(N(msiguser3), "active"));

    signature signatures{{std::make_tuple(msiguser1_sig, pbtx_KeyType_EOSIO_KEY, msiguser1_sig.size()),
                          std::make_tuple(msiguser2_sig, pbtx_KeyType_EOSIO_KEY, msiguser2_sig.size()),
                          std::make_tuple(msiguser3_sig, pbtx_KeyType_EOSIO_KEY, msiguser3_sig.size())}};

    auto [status, trx] = encode_transaction(encoded_trx_body, signatures.size(), signatures);
    BOOST_REQUIRE_EQUAL(true, status);

    BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.exectrx(N(bob), N(bob), trx));

}
FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
