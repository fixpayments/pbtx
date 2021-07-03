#include "pbtx_tester.hpp"

BOOST_AUTO_TEST_SUITE(cleanhistory_tests)

BOOST_FIXTURE_TEST_CASE(asserts_cleanhistory_test, pbtx_tester)
try
{
    BOOST_REQUIRE_EQUAL(error("missing authority of bob"), 
                        m_pbtx_api.cleanhistory(N(alice), 1000, 10, 10));

    BOOST_REQUIRE_EQUAL(wasm_assert_msg("Unknown network"),
                        m_pbtx_api.cleanhistory(N(bob), 666, 10, 10));

    BOOST_REQUIRE_EQUAL(wasm_assert_msg("Nothing to clean"),
                        m_pbtx_api.cleanhistory(N(bob), 1000, 10, 10));    
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(partial_cleanhistory_test, pbtx_tester)
try
{
    BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.netmetadata(N(bob), 999, {55, 55 , 55, 55, 55}));
    BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.cleanhistory(N(bob), 999, 1, 1));

    auto history = m_pbtx_api.get_history(999, 2);

    REQUIRE_MATCHING_OBJECT(history, mvo()
    ("id", 2)
    ("event_type", PBTX_HISTORY_EVENT_NETMETADATA)
    ("data", std::vector<uint8_t>{55, 55 , 55, 55, 55})
    ("trx_id", "4f250f7e5ab0459f2bcf54d6743074ed8de2e8bca97cd70c22b149245f693bd4")
    ("trx_time", "2020-01-01T00:00:08.000"));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(full_cleanhistory_test, pbtx_tester)
try
{
    BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.netmetadata(N(bob), 999, {55, 55 , 55, 55, 55}));
    BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.cleanhistory(N(bob), 999, 10, 10));

    auto history = m_pbtx_api.get_history(999, 1);
    BOOST_REQUIRE_EQUAL(true, history.is_null());
}
FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
