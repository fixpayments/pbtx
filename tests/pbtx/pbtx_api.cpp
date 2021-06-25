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
    vector<char> data = _tester->get_row_by_account(contract, account_name(0), N(networks), account_name(network_id));
    return data.empty() ? fc::variant() : abi_ser.binary_to_variant("networks_row", data, base_tester::abi_serializer_max_time);
}

fc::variant pbtx_api::get_actor_permission(const uint64_t &network_id, const uint64_t &actor)
{
	vector<char> data = _tester->get_row_by_account(contract, account_name(network_id), N(actorperm), account_name(actor));
    return data.empty() ? fc::variant() : abi_ser.binary_to_variant("actorperm_row", data, base_tester::abi_serializer_max_time);
}

fc::variant pbtx_api::get_actor_sequence(const uint64_t &network_id, const uint64_t &actor)
{
	vector<char> data = _tester->get_row_by_account(contract, account_name(network_id), N(actorseq), account_name(actor));
    return data.empty() ? fc::variant() : abi_ser.binary_to_variant("actorseq_row", data, base_tester::abi_serializer_max_time);
}

fc::variant pbtx_api::get_metadata(const uint64_t &network_id)
{
	vector<char> data = _tester->get_row_by_account(contract, account_name(0), N(netmetadata), account_name(network_id));
    return data.empty() ? fc::variant() : abi_ser.binary_to_variant("netmd_row", data, base_tester::abi_serializer_max_time);
}

fc::variant pbtx_api::get_history_id(const uint64_t &network_id)
{
	vector<char> data = _tester->get_row_by_account(contract, account_name(0), N(histid), account_name(network_id));
    return data.empty() ? fc::variant() : abi_ser.binary_to_variant("histid_row", data, base_tester::abi_serializer_max_time);
}

fc::variant pbtx_api::get_history(const uint64_t &network_id, const uint64_t &id)
{
	vector<char> data = _tester->get_row_by_account(contract, account_name(network_id), N(history), account_name(id));
    return data.empty() ? fc::variant() : abi_ser.binary_to_variant("history_row", data, base_tester::abi_serializer_max_time);
}

action_result pbtx_api::regnetwork(const account_name &signer, const uint64_t &network_id,
								   const name &admin_acc, const vector<name> &listeners, const uint32_t &flags)
{
	return push_action(signer, contract, N(regnetwork), mvo()("network_id", network_id)("admin_acc", admin_acc)("listeners", listeners)("flags", flags));
}

action_result pbtx_api::netmetadata(const account_name &signer, const uint64_t &network_id, const std::vector<uint8_t> &metadata)
{
	return push_action(signer, contract, N(netmetadata), mvo()("network_id", network_id)("metadata", metadata));
}

action_result pbtx_api::unregnetwork(const account_name &signer, const uint64_t &network_id)
{
    return push_action(signer, contract, N(unregnetwork), mvo()("network_id", network_id));
}

action_result pbtx_api::regactor(const account_name &signer, const uint64_t &network_id, const vector<uint8_t> &permission)
{
    return push_action(signer, contract, N(regactor), mvo()("network_id", network_id)("permission", permission));
}

action_result pbtx_api::unregactor(const account_name &signer, const uint64_t &network_id, const uint64_t &actor)
{
    return push_action(signer, contract, N(unregactor), mvo()("network_id", network_id)("actor", actor));
}

action_result pbtx_api::exectrx(const account_name &signer, const account_name &worker, const vector<uint8_t> &trx_input)
{
    return push_action(signer, contract, N(exectrx), mvo()("worker", worker)("trx_input", trx_input));
}

action_result pbtx_api::cleanhistory(const account_name &signer, const uint64_t &network_id, const uint64_t &upto_id, const uint32_t &maxrows)
{
    return push_action(signer, contract, N(cleanhistory), mvo()("network_id", network_id)("upto_id", upto_id)("maxrows", maxrows));
}

action_result pbtx_api::push_action(const name& signer, const name& cnt, const action_name& name, const variant_object& data) {
	string action_type_name = abi_ser.get_action_type(name);
	action act;
	act.account = cnt;
	act.name = name;
	act.data = abi_ser.variant_to_binary(action_type_name, data, base_tester::abi_serializer_max_time);

	return _tester->push_action(std::move(act), signer.to_uint64_t());
}
