#include "pbtx_tester.hpp"

BOOST_AUTO_TEST_SUITE(unregactor_tests)

BOOST_FIXTURE_TEST_CASE(asserts_unregactor_test, pbtx_tester)
try
{
    BOOST_REQUIRE_EQUAL(error("missing authority of bob"), 
                        m_pbtx_api.unregactor(N(alice), 1000, {}));

    BOOST_REQUIRE_EQUAL(wasm_assert_msg("Unknown network"),
                        m_pbtx_api.unregactor(N(alice), 666, {}));

    BOOST_REQUIRE_EQUAL(wasm_assert_msg("TUnknown actor"),
                        m_pbtx_api.unregactor(N(alice), 1000, 666));

    BOOST_REQUIRE_EQUAL(wasm_assert_msg("Exception 1"),
                        m_pbtx_api.unregactor(N(alice), 666, {}));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(unregactor_test, pbtx_tester)
try
{
    BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.unregactor(N(alice), 1000, 1000));

    auto permission = m_pbtx_api.get_actor_permission(1000, 1002);
    BOOST_REQUIRE_EQUAL(true, permission.is_null());

    auto sequence = m_pbtx_api.get_actor_sequence(1000, 1002);
    BOOST_REQUIRE_EQUAL(true, sequence.is_null());
}
FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
