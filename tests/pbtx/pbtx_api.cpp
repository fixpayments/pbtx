#include "pbtx_api.hpp"

pbtx_api::pbtx_api(name acnt, tester* tester)
{
    contract = acnt;
	_tester = tester;

	_tester->create_accounts({contract});

    _tester->produce_blocks(2);

	_tester->set_code(contract, contracts::pbtx_wasm());
	_tester->set_abi(contract, contracts::pbtx_abi().data());

    _tester->produce_blocks();

	const auto& accnt = _tester->control->db().get<account_object, by_name>(contract);

	abi_def abi;
	BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt.abi, abi), true);
	abi_ser.set_abi(abi, base_tester::abi_serializer_max_time);
}

fc::variant pbtx_api::get_network(const uint64_t &network_id)
{

}

fc::variant pbtx_api::get_actor_permission(const uint64_t &network_id, const uint64_t &actor)
{

}

fc::variant pbtx_api::get_actor_sequence(const uint64_t &network_id, const uint64_t &actor)
{

}

action_result pbtx_api::regnetwork(const account_name &signer, const uint64_t &network_id,
                            const name &admin_acc, const vector<name> &listeners, const uint32_t &flags)
{

}

action_result pbtx_api::unregnetwrok(const account_name &signer, const uint64_t &network_id)
{
    return push_action(signer, contract, N(exectrx), mvo()("trx_input", trx_input));
}

action_result pbtx_api::regactor(const account_name &signer, const uint64_t &network_id, const vector<uint8_t> &permission)
{
    return push_action(signer, contract, N(regactor), mvo()("network_id", network_id)("permission", permission));
}

action_result pbtx_api::unregactor(const account_name &signer, const uint64_t &network_id, const uint64_t &actor)
{
    return push_action(signer, contract, N(unregactor), mvo()("network_id", network_id)("actor", actor));
}

action_result pbtx_api::exectrx(const account_name &signer, const vector<uint8_t> &trx_input)
{
    return push_action(signer, contract, N(exectrx), mvo()("trx_input", trx_input));
}

action_result pbtx_api::push_action(const name& signer, const name& cnt, const action_name& name, const variant_object& data) {
	string action_type_name = abi_ser.get_action_type(name);
	action act;
	act.account = cnt;
	act.name = name;
	act.data = abi_ser.variant_to_binary(action_type_name, data, base_tester::abi_serializer_max_time);

	return _tester->push_action(std::move(act), signer.to_uint64_t());
}
