#include "sgx_contract.h"

SGX_Contract::SGX_Contract() {};

void SGX_Contract::loadContract(nlohmann::json contract_definition, eevm::Address *owner) {
    DEBUG("");
    // Create environment
    eevm::SimpleGlobalState *gs = new eevm::SimpleGlobalState();

    env = new Environment{gs, *owner, contract_definition};

    // Deploy the lntee contract
    contract_address = deploy_lnSGX_Contract();
}

int SGX_Contract::rand_range(int exclusive_upper_bound) {
    std::random_device rand_device;
    std::mt19937 generator(rand_device());
    std::uniform_int_distribution<size_t> dist(0, exclusive_upper_bound - 1);

    return dist(generator);
}

uint256_t SGX_Contract::get_random_uint256(size_t bytes) {
    std::vector<uint8_t> raw(bytes);
    std::generate(raw.begin(), raw.end(), []() { return rand(); });
    return eevm::from_big_endian(raw.data(), raw.size());
}

///////////////////////////////////////////////////////////////////////////////

// Truncate 160-bit addresses to a more human-friendly length, retaining the
// start and end for identification
std::string SGX_Contract::short_name(const eevm::Address &address) {
    const auto full_hex = eevm::to_hex_string(address);
    return full_hex.substr(0, 5) + std::string("...") +
           full_hex.substr(full_hex.size() - 3);
}

// Run input as an EVM transaction, check the result and return the output
std::vector<uint8_t> SGX_Contract::run_and_check_result(
        const eevm::Address &from, const eevm::Address &to, const eevm::Code &input) {
//    DEBUG("");
    // Ignore any logs produced by this transaction
    eevm::NullLogHandler ignore;
    eevm::Transaction tx(from, ignore, to);
    // Record a trace to aid debugging
    eevm::Trace tr;
    eevm::Processor p(*(env->gs));
//    DEBUG("");
    // Run the transaction
//    std::cout << "from = " << eevm::to_hex_string(from) << std::endl;
//    std::cout << "to = " << eevm::to_hex_string(to) << std::endl;

    const auto exec_result = p.run(tx, from, env->gs->get(to), input, 0u, &tr);

//    p.run(tx, from, env->gs->get(to), input, 0u, &tr);


//    const auto exec_result = p.run(tx, from, env.gs.get(to), input, 0u, &tr);
//    DEBUG("");
    if (exec_result.er != eevm::ExitReason::returned) {
        std::cout <<"ERRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR"<<std::endl;
        // Print the trace if nothing was returned
        std::cerr << fmt::format("Trace:\n{}", tr) << std::endl;
        INFO();
        if (exec_result.er == eevm::ExitReason::threw) {
            INFO();
            std::cout << "ERROR" << std::endl;
            // Rethrow to highlight any exceptions raised in execution
//            throw std::runtime_error(
//                    fmt::format("Execution threw an error: {}", exec_result.exmsg));
        }
        INFO();
        std::cout <<"Deployment did not return"<<std::endl;
    }
    INFO();
    DEBUG("");
    return exec_result.output;
}

// Modify code to append ABI-encoding of arg, suitable for passing to contract
// execution
void SGX_Contract::append_argument(
        std::vector<uint8_t> &code,
        const uint256_t &arg) {
    const auto pre_size = code.size();
    code.resize(pre_size + 32u);
    eevm::to_big_endian(arg, code.data() + pre_size);
}

/**
 * Deploy the lntee contract defined in env, with total_supply tokens. Return
 * the address the contract was deployed to
 * @return
 */
eevm::Address SGX_Contract::deploy_lnSGX_Contract() {
    /** Generate the contract address */
    const auto contract_address = eevm::generate_address(env->owner_address, 0u);
    /** Get the binary constructor of the contract */
    auto contract_constructor = eevm::to_bytes(env->contract_definition["bin"]);
    /** The constructor takes a single argument (total_supply) - append it */
    append_argument(contract_constructor, total_supply);
    /** Set this constructor as the contract's code body */
    auto contract = env->gs->create(contract_address, 0u, contract_constructor);
    INFO();
    /** Run a transaction to initialise this account */
    auto result = run_and_check_result(env->owner_address, contract_address, {});

    /** Result of running the compiled constructor is the code that should be the */
    /** contract's body (constructor will also have setup contract's Storage) */
    contract.acc.set_code(std::move(result));

    return contract.acc.get_address();
}

// Get the total token supply by calling totalSupply on the contract_address
uint256_t SGX_Contract::get_total_supply() {
    // Anyone can call totalSupply - prove this by asking from a randomly
    // generated address
    // const auto caller = get_random_address();
    const auto function_call =
            eevm::to_bytes(env->contract_definition["hashes"]["totalSupply()"]);
    INFO();
    const auto output =
            run_and_check_result(env->owner_address, contract_address, function_call);
    return eevm::from_big_endian(output.data(), output.size());
}

// Get the current token balance of target_address by calling balanceOf on
// contract_address
uint256_t SGX_Contract::get_balance(const eevm::Address &target_address) {
    // Anyone can call balanceOf
    auto function_call =
            eevm::to_bytes(env->contract_definition["hashes"]["balanceOf(address)"]);
    append_argument(function_call, target_address);
INFO();
    const auto output =
            run_and_check_result(target_address, contract_address, function_call);
    return eevm::from_big_endian(output.data(), output.size());
}

// Transfer tokens from source_address to target_address by calling transfer on
// contract_address
bool SGX_Contract::transfer(
        const eevm::Address &source_address,
        const eevm::Address &target_address,
        const uint256_t &amount) {
    DEBUG("");
    // To transfer tokens, the caller must be the intended source address
    auto function_call = eevm::to_bytes(
            env->contract_definition["hashes"]["transfer(address,uint256)"]);
    append_argument(function_call, target_address);
    memset(((char *) &amount) + 8, 0, 24);
    append_argument(function_call, amount);
    return invoke(env->owner_address, function_call);
}

bool SGX_Contract::invoke(
        const eevm::Address &source_address, eevm::Code &function_call) {
    DEBUG("");
//    std::cout << fmt::format(
//            "<><><><><><<><{}",
//            eevm::to_lower_hex_string(function_call)) << std::endl;
//
//
//    std::cout << fmt::format(
//            "Transferring {} from {} to {}",
//            eevm::to_lower_hex_string(function_call),
//            eevm::to_checksum_address(source_address),
//            eevm::to_checksum_address(contract_address))
//              << std::endl;
INFO();
    const auto output =
            run_and_check_result(source_address, contract_address, function_call);

    if (output.size() != 32 || (output[31] != 0 && output[31] != 1)) {
        throw std::runtime_error("Unexpected output from call to transfer");
    }
    return output[31] == 1;
}

// Print the total token supply and current token balance of each user, by
// sending transactions to the given contract_address
void SGX_Contract::print_lntee_state(const std::string &heading) {
    const auto total_supply = get_total_supply();
    using Balances = std::vector<std::pair<eevm::Address, uint256_t>>;
    Balances balances;
    for (const auto &user : users) {
        uint256_t balance = get_balance(user);
        balances.emplace_back(std::make_pair(user, balance));
    }

    std::cout << heading << std::endl;
    std::cout << "Total supply of tokens is: " << total_supply << std::endl;
    std::cout << "User balances: " << std::endl;

    for (const auto &pair : balances) {
        std::cout << "  " << pair.second << " owned by " << short_name(pair.first);
        if (pair.first == env->owner_address) {
            std::cout << " (original contract creator)";
        }
        std::cout << std::endl;
    }

    std::cout << std::string(heading.size(), '-') << std::endl;
}