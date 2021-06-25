#include "pbtx_tester.hpp"

BOOST_AUTO_TEST_SUITE(newmetadata_tests)

BOOST_FIXTURE_TEST_CASE(asserts_newmetadata_test, pbtx_tester)
try
{
    BOOST_REQUIRE_EQUAL(error("missing authority of bob"), 
                        m_pbtx_api.netmetadata(N(alice), 1000, {}));

    BOOST_REQUIRE_EQUAL(wasm_assert_msg("Unknown network"),
                        m_pbtx_api.netmetadata(N(bob), 666, {}));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(newmetadata_test, pbtx_tester)
try
{
    std::vector<uint8_t> data{55, 55, 55, 55, 55};
    BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.netmetadata(N(bob), 1000, data));

    auto meta_data = m_pbtx_api.get_metadata(1000);

    REQUIRE_MATCHING_OBJECT(meta_data, mvo()
    ("network_id", 1000)
    ("data", data));

    std::vector<uint8_t> new_data{33, 33, 33, 33, 33};
    BOOST_REQUIRE_EQUAL(success(), m_pbtx_api.netmetadata(N(bob), 1000, new_data));

    meta_data = m_pbtx_api.get_metadata(1000);

    REQUIRE_MATCHING_OBJECT(meta_data, mvo()
    ("network_id", 1000)
    ("data", new_data));
}
FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
