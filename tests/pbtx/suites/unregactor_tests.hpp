#include "pbtx_tester.hpp"

BOOST_AUTO_TEST_SUITE(unregactor_tests)

BOOST_FIXTURE_TEST_CASE(asserts_unregactor_test, pbtx_tester)
try
{
    BOOST_REQUIRE_EQUAL(error("missing authority of bob"), 
                        m_pbtx_api.unregactor(N(alice), 1000, {}));

    BOOST_REQUIRE_EQUAL(wasm_assert_msg("Unknown network"),
                        m_pbtx_api.unregactor(N(alice), 666, {}));

    BOOST_REQUIRE_EQUAL(wasm_assert_msg("Unknown actor"),
                        m_pbtx_api.unregactor(N(bob), 1001, string_to_uint64_t("bob")));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(no_history_unregactor_test, pbtx_tester)
try
{
    BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.unregactor(N(bob), 1001, string_to_uint64_t("alice")));

    auto permission = m_pbtx_api.get_actor_permission(1001, string_to_uint64_t("alice"));
    BOOST_REQUIRE_EQUAL(true, permission.is_null());

    auto sequence = m_pbtx_api.get_actor_sequence(1001, string_to_uint64_t("alice"));
    BOOST_REQUIRE_EQUAL(true, sequence.is_null());

    auto history_id = m_pbtx_api.get_history_id(1001);
    BOOST_REQUIRE_EQUAL(true, history_id.is_null());

    auto history = m_pbtx_api.get_history(1001, 1);
    BOOST_REQUIRE_EQUAL(true, history.is_null());
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(add_history_unregactor_test, pbtx_tester)
try
{
    auto permission = m_pbtx_api.get_actor_permission(999, string_to_uint64_t("alice"));

    BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.unregactor(N(bob), 999, string_to_uint64_t("alice")));

    auto actor_permission = m_pbtx_api.get_actor_permission(999, string_to_uint64_t("alice"));
    BOOST_REQUIRE_EQUAL(true, actor_permission.is_null());

    auto sequence = m_pbtx_api.get_actor_sequence(999, string_to_uint64_t("alice"));
    BOOST_REQUIRE_EQUAL(true, sequence.is_null());

    auto history_id = m_pbtx_api.get_history_id(999);

    REQUIRE_MATCHING_OBJECT(history_id, mvo()
    ("network_id", 999)
    ("last_history_id", 2));

    auto history = m_pbtx_api.get_history(999, history_id["last_history_id"].as_uint64());

    REQUIRE_MATCHING_OBJECT(history, mvo()
    ("id", history_id["last_history_id"].as_uint64())
    ("event_type", PBTX_HISTORY_EVENT_UNREGACTOR)
    ("data", permission["permission"].get_array())
    ("trx_id", "c0647fa38b9f037219ee3f2f5da06f5f8ab8c33eee4b9f6bc97658205ac1a458")
    ("trx_time", "2020-01-01T00:00:07.000"));
}
FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
