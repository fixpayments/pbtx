#include "pbtx_tester.hpp"

BOOST_AUTO_TEST_SUITE(regnetwork_tests)

BOOST_FIXTURE_TEST_CASE(asserts_regnetwork_test, pbtx_tester)
try
{
    BOOST_REQUIRE_EQUAL(error("missing authority of bob"), 
                        m_pbtx_api.regnetwork(N(alice), 1002, N(bob), {},  0xFFFFFFFF));

    BOOST_REQUIRE_EQUAL(wasm_assert_msg("Unrecognized bits set in lower 16 bits of flags"),
                        m_pbtx_api.regnetwork(N(bob), 1002, N(bob), {},  0x00000000));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(regnetwork_test, pbtx_tester)
try
{
    BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.regnetwork(N(bob), 1002, N(bob), {},  0x00000001));

    auto network = m_pbtx_api.get_network(1002);

    REQUIRE_MATCHING_OBJECT(network, mvo()
    ("network_id", "")
    ("admin_acc", "")
    ("listeners", "")
    ("flags", ""));
}
FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
