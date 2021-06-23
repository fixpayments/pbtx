#include "pbtx_tester.hpp"

BOOST_AUTO_TEST_SUITE(exectrx_tests)

BOOST_FIXTURE_TEST_CASE(asserts_exectrx_test, pbtx_tester)
try
{
    BOOST_REQUIRE_EQUAL(error("missing authority of bob"), 
                        m_pbtx_api.exectrx(N(alice), N(bob), {}));

    // BOOST_REQUIRE_EQUAL(wasm_assert_msg("Empty transaction body"),
    //                     m_pbtx_api.exectrx(N(alice), N(), {}));

    // BOOST_REQUIRE_EQUAL(wasm_assert_msg("Unknown network_id: "),
    //                     m_pbtx_api.exectrx(N(alice), N(), {}));

    // BOOST_REQUIRE_EQUAL(wasm_assert_msg("Unknown actor: "),
    //                     m_pbtx_api.exectrx(N(alice), N(), {}));

    // BOOST_REQUIRE_EQUAL(wasm_assert_msg("Expected seqnum="),
    //                     m_pbtx_api.exectrx(N(alice), N(), {}));

    // BOOST_REQUIRE_EQUAL(wasm_assert_msg("Previous body hash mismatch. Expected "),
    //                     m_pbtx_api.exectrx(N(alice), N(), {}));

    // BOOST_REQUIRE_EQUAL(wasm_assert_msg("Expected "),
    //                     m_pbtx_api.exectrx(N(alice), N(), {}));

    // BOOST_REQUIRE_EQUAL(wasm_assert_msg("Unknown cosignor #"),
    //                     m_pbtx_api.exectrx(N(alice), N(), {}));


    // BOOST_REQUIRE_EQUAL(wasm_assert_msg("Signature too short. Expected 65 bytes or more, got "),
    //                     m_pbtx_api.exectrx(N(alice), N(), {}));

    // BOOST_REQUIRE_EQUAL(wasm_assert_msg("Signature #  does not match any keys of actor #"),
    //                     m_pbtx_api.exectrx(N(alice), N(), {}));

    // BOOST_REQUIRE_EQUAL(wasm_assert_msg("Insufficient signatures weight for actor #"),
    //                     m_pbtx_api.exectrx(N(alice), N(), {}));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(exectrx_test, pbtx_tester)
try
{

    BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.exectrx(N(bob), N(bob), {}));
    
}
FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
