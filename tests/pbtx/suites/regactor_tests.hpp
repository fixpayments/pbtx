#include "pbtx_tester.hpp"

BOOST_AUTO_TEST_SUITE(regactor_tests)

BOOST_FIXTURE_TEST_CASE(asserts_regactor_test, pbtx_tester)
try
{
    BOOST_REQUIRE_EQUAL(error("missing authority of bob"), 
                        m_pbtx_api.regactor(N(alice), 1000, {}));

    BOOST_REQUIRE_EQUAL(wasm_assert_msg("Unknown network"),
                        m_pbtx_api.regactor(N(bob), 666, {}));

    BOOST_REQUIRE_EQUAL(wasm_assert_msg("Threshold cannot be zero"),
                        m_pbtx_api.regactor(N(bob), 1000, {}));

    key keys2{{fc::raw::pack(get_public_key(N(alice), "active")), pbtx_KeyType_EOSIO_KEY, 0}};
    auto permission2 = encode_permisson(string_to_uint64_t("alice"), 1, keys2.size(), keys2);

    BOOST_REQUIRE_EQUAL(wasm_assert_msg("Key weight cannot be zero in key #0"),
                        m_pbtx_api.regactor(N(bob), 1000, permission2));

    key keys3{{fc::raw::pack(get_public_key(N(alice), "active")), (pbtx_KeyType)5, 1}};
    auto permission3 = encode_permisson(string_to_uint64_t("alice"), 1, keys3.size(), keys3);

    BOOST_REQUIRE_EQUAL(wasm_assert_msg("Unknown key type: 5 in key #0"),
                        m_pbtx_api.regactor(N(bob), 1000, permission3));

    std::vector<char> shortkey {0, 55, 55, 55, 55, 55, 55, 55, 55};
    key keys4{{shortkey, pbtx_KeyType_EOSIO_KEY, 1}};
    auto permission4 = encode_permisson(string_to_uint64_t("alice"), 1, keys4.size(), keys4);

    BOOST_REQUIRE_EQUAL(wasm_assert_msg("Key #0 is too short"),
                        m_pbtx_api.regactor(N(bob), 1000, permission4));
    
    key keys5{{fc::raw::pack(get_public_key(N(alice), "active")), pbtx_KeyType_EOSIO_KEY, 1}};
    auto permission5 = encode_permisson(string_to_uint64_t("alice"), 2, keys5.size(), keys5);

    BOOST_REQUIRE_EQUAL(wasm_assert_msg("Threshold cannot be higher than sum of weights"),
                        m_pbtx_api.regactor(N(bob), 1000, permission5));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(no_history_regactor_test, pbtx_tester)
try
{
    uint64_t network_id = 1000;
    auto actor = string_to_uint64_t("alice");
    key keys{{fc::raw::pack(get_public_key(N(alice), "active")), pbtx_KeyType_EOSIO_KEY, 1}};
    auto permission = encode_permisson(actor, 1, keys.size(), keys);
  
    BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.regactor(N(bob), network_id, permission));

    auto actor_permission = m_pbtx_api.get_actor_permission(network_id, actor);

    REQUIRE_MATCHING_OBJECT(actor_permission, mvo()
    ("actor", actor)
    ("permission", permission));

    auto actor_sequence = m_pbtx_api.get_actor_sequence(network_id, actor);

    REQUIRE_MATCHING_OBJECT(actor_sequence, mvo()
    ("actor", actor)
    ("seqnum", 0)
    ("prev_hash", 0)
    ("last_modified", "1970-01-01T00:00:00.000"));

    auto history_id = m_pbtx_api.get_history_id(1000);
    BOOST_REQUIRE_EQUAL(true, history_id.is_null());

    auto history = m_pbtx_api.get_history(1000, 1);
    BOOST_REQUIRE_EQUAL(true, history.is_null());
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(add_history_regactor_test, pbtx_tester)
try
{
    uint64_t network_id = 998;
    auto actor = string_to_uint64_t("alice");
    key keys{{fc::raw::pack(get_public_key(N(alice), "active")), pbtx_KeyType_EOSIO_KEY, 1}};
    auto permission = encode_permisson(actor, 1, keys.size(), keys);
  
    BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.regactor(N(bob), network_id, permission));

    auto actor_permission = m_pbtx_api.get_actor_permission(network_id, actor);

    REQUIRE_MATCHING_OBJECT(actor_permission, mvo()
    ("actor", actor)
    ("permission", permission));

    auto actor_sequence = m_pbtx_api.get_actor_sequence(network_id, actor);

    REQUIRE_MATCHING_OBJECT(actor_sequence, mvo()
    ("actor", actor)
    ("seqnum", 0)
    ("prev_hash", 0)
    ("last_modified", "1970-01-01T00:00:00.000"));

    auto history_id = m_pbtx_api.get_history_id(998);

    REQUIRE_MATCHING_OBJECT(history_id, mvo()
    ("network_id", 998)
    ("last_history_id", 1));

    auto history = m_pbtx_api.get_history(998, history_id["last_history_id"].as_uint64());

    REQUIRE_MATCHING_OBJECT(history, mvo()
    ("id", history_id["last_history_id"].as_uint64())
    ("event_type", PBTX_HISTORY_EVENT_REGACTOR)
    ("data", permission)
    ("trx_id", "3766c444abe4d313ec6240df8d2795a52814d8a365c4d3efd6bfc0918a7795b7")
    ("trx_time", "2020-01-01T00:00:07.000"));
}
FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
