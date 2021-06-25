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

BOOST_FIXTURE_TEST_CASE(cleanhistory_test, pbtx_tester)
try
{
    BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.netmetadata(N(bob), 1000, {55, 55 , 55, 55, 55}));
    // BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.cleanhistory(N(bob), 1000, 10, 10));

    // auto history = m_pbtx_api.get_history(1000, 1);

    // std::cout << history << std::endl;
    // BOOST_REQUIRE_EQUAL(true, history.is_null());
}
FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
