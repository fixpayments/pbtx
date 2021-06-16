#include "pbtx_tester.hpp"

BOOST_AUTO_TEST_SUITE(unregnetwork_tests)

BOOST_FIXTURE_TEST_CASE(asserts_unregnetwork_test, pbtx_tester)
try
{
    BOOST_REQUIRE_EQUAL(error("missing authority of bob"), 
                        m_pbtx_api.unregnetwork(N(alice), 1000));

    BOOST_REQUIRE_EQUAL(wasm_assert_msg("Unknown network"),
                        m_pbtx_api.unregnetwork(N(bob), 1000));

     BOOST_REQUIRE_EQUAL(wasm_assert_msg("Cannot delete a network while it has actors"),
                        m_pbtx_api.unregnetwork(N(bob), 1000));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(unregnetwork_test, pbtx_tester)
try
{
    BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.unregnetwork(N(bob), 1000));

    auto network = m_pbtx_api.get_network(1000);
    BOOST_REQUIRE_EQUAL(true, network.is_null());
}
FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
