#include "pbtx_tester.hpp"

pbtx_tester::pbtx_tester()
	: m_pbtx_api(N(pbtx), this)
{
	produce_blocks(2);

    create_accounts({
        N(alice),
        N(bob),
        N(carol)
    });
}

void pbtx_tester::pbtx_init()
{
    
}
