#include "pbtx_tester.hpp"

BOOST_AUTO_TEST_SUITE(regnetwork_tests)

BOOST_FIXTURE_TEST_CASE(asserts_regnetwork_test, pbtx_tester)
try
{
    BOOST_REQUIRE_EQUAL(error("missing authority of bob"), 
                        m_pbtx_api.regnetwork(N(alice), 1002, N(bob), {},  0xFFFF0000));

    BOOST_REQUIRE_EQUAL(wasm_assert_msg("Unrecognized bits set in lower 16 bits of flags"),
                        m_pbtx_api.regnetwork(N(bob), 1002, N(bob), {},  0xFFFF0010));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(regnetwork_test, pbtx_tester)
try
{
    BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.regnetwork(N(bob), 1002, N(bob), {},  0xFFFF0001));

    auto network = m_pbtx_api.get_network(1002);

    BOOST_REQUIRE_EQUAL(network["network_id"], fc::variant("1002"));
    BOOST_REQUIRE_EQUAL(network["admin_acc"], fc::variant("bob"));
    BOOST_REQUIRE_EQUAL(network["listeners"].get_array().size(), fc::variant("0"));
    BOOST_REQUIRE_EQUAL(network["flags"], fc::variant("4294901761"));
}
FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
