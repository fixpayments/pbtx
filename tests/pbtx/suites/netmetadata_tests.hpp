#include "pbtx_tester.hpp"

BOOST_AUTO_TEST_SUITE(netmetadata_tests)

BOOST_FIXTURE_TEST_CASE(asserts_netmetadata_test, pbtx_tester)
try
{
    BOOST_REQUIRE_EQUAL(error("missing authority of bob"), 
                        m_pbtx_api.netmetadata(N(alice), 1000, {}));

    BOOST_REQUIRE_EQUAL(wasm_assert_msg("Unknown network"),
                        m_pbtx_api.netmetadata(N(bob), 666, {}));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(no_history_netmetadata_test, pbtx_tester)
try
{   
    std::vector<uint8_t> data{55, 55, 55, 55, 55};
    BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.netmetadata(N(bob), 1000, data));

    auto meta_data = m_pbtx_api.get_metadata(1000);

    REQUIRE_MATCHING_OBJECT(meta_data, mvo()
    ("network_id", 1000)
    ("data", data));

    auto history_id = m_pbtx_api.get_history_id(1000);
    BOOST_REQUIRE_EQUAL(true, history_id.is_null());

    auto history = m_pbtx_api.get_history(1000, 1);
    BOOST_REQUIRE_EQUAL(true, history.is_null());
    
    std::vector<uint8_t> new_data{33, 33, 33, 33, 33};
    BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.netmetadata(N(bob), 1000, new_data));

    meta_data = m_pbtx_api.get_metadata(1000);

    REQUIRE_MATCHING_OBJECT(meta_data, mvo()
    ("network_id", 1000)
    ("data", new_data));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(add_history_netmetadata_test, pbtx_tester)
try
{
    std::vector<uint8_t> data{55, 55, 55, 55, 55};
    BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.netmetadata(N(bob), 998, data));

    auto meta_data = m_pbtx_api.get_metadata(998);

    REQUIRE_MATCHING_OBJECT(meta_data, mvo()
    ("network_id", 998)
    ("data", data));

    auto history_id = m_pbtx_api.get_history_id(998);

    REQUIRE_MATCHING_OBJECT(history_id, mvo()
    ("network_id", 998)
    ("last_history_id", 1));

    auto history = m_pbtx_api.get_history(998, history_id["last_history_id"].as_uint64());

    REQUIRE_MATCHING_OBJECT(history, mvo()
    ("id", history_id["last_history_id"].as_uint64())
    ("event_type", PBTX_HISTORY_EVENT_NETMETADATA)
    ("data", data)
    ("trx_id", "8b88127a5adc894d5464a85860c5b72ccb7f812ca7e92b35be9e710cd2cbf504")
    ("trx_time", "2020-01-01T00:00:07.000"));
}
FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
