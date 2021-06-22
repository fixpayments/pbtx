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

    key keys2{{get_public_key(N(alice), "active"), pbtx_KeyType_EOSIO_KEY, 0}};
    auto permission2 = encode_permisson(string_to_uint64_t("alice"), 1, keys2.size(), keys2);

    BOOST_REQUIRE_EQUAL(wasm_assert_msg("Key weight cannot be zero in key #0"),
                        m_pbtx_api.regactor(N(bob), 1000, permission2));

    // key keys3{{get_public_key(N(alice), "active"), pbtx_KeyType_EOSIO_KEY, 1}};
    // auto permission3 = encode_permisson(string_to_uint64_t("alice"), 1, keys3.size(), keys3);

    // permission3[19] = 1;

    // BOOST_REQUIRE_EQUAL(wasm_assert_msg("Unknown key type:"),
    //                     m_pbtx_api.regactor(N(bob), 1000, permission3));

    // std::string str_key("EOS5hV1LkHKpp3q2acWnaoFHuev1vpzjDyTXsWDffgehXFhZk45gt");
    // key keys4{{fc::crypto::public_key(str_key), pbtx_KeyType_EOSIO_KEY, 1}};
    // auto permission4 = encode_permisson(string_to_uint64_t("alice"), 1, keys4.size(), keys4);

    // BOOST_REQUIRE_EQUAL(wasm_assert_msg("Key # is too short"),
    //                     m_pbtx_api.regactor(N(bob), 1000, permission4));

    key keys5{{get_public_key(N(alice), "active"), pbtx_KeyType_EOSIO_KEY, 1}};
    auto permission5 = encode_permisson(string_to_uint64_t("alice"), 2, keys5.size(), keys5);

    BOOST_REQUIRE_EQUAL(wasm_assert_msg("Threshold cannot be higher than sum of weights"),
                        m_pbtx_api.regactor(N(bob), 1000, permission5));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(regactor_test, pbtx_tester)
try
{
    uint64_t network_id = 1000;
    auto actor = string_to_uint64_t("alice");
    key keys{{get_public_key(N(alice), "active"), pbtx_KeyType_EOSIO_KEY, 1}};
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
}
FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
