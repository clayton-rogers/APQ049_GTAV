#include <iostream>
#include <vector>
#include <cstdint>
#include <algorithm>

constexpr long BREAK_THRES = 1'000'000'000;
constexpr long PRUNE_THRESHOLD = 10000000;


// 1 BILLION DOLLARS IN GTA V
//
// A: 52 seconds, 25'000 $
// B: 18 seconds, Gives locked in amount
// C: 10 mins (600 seconds), Saves current amount

struct state_t {
    long money = 0;
    long saved_money = 0;
    long time_seconds = 0;
    std::vector<char> method_history;
    bool has_saved = false;
};

state_t method_a(const state_t& state)
{
    state_t new_state = state;
    new_state.money += 25000;
    new_state.time_seconds += 52;

    new_state.method_history.push_back('a');

    return new_state;
}

state_t method_b(const state_t& state)
{
    state_t new_state = state;
    new_state.money += state.saved_money;
    new_state.time_seconds += 18;

    new_state.method_history.push_back('b');

    return new_state;
}

state_t method_c(const state_t& state)
{
    state_t new_state = state;
    new_state.saved_money = state.money;
    new_state.time_seconds += 600;
    new_state.has_saved = true;

    new_state.method_history.push_back('c');

    return new_state;
}

void print_state(const state_t& state)
{
    std::cout << "time: " << state.time_seconds << " money: " << state.money << " money/time: " << state.money / state.time_seconds << " ";
    for (const auto& c : state.method_history) {
        std::cout << c;
    }
    std::cout << std::endl;
}

void print_all(const std::vector<state_t>& states)
{
    for (const auto& state : states) {
        print_state(state);
    }
}

std::vector<state_t> do_iter(const std::vector<state_t>& states)
{
    std::vector<state_t> ret_val;

    for (const auto& state : states) {
        // Assume that we only do at at the start, and never after saving
        // to reduce problem space
        if (!state.has_saved) {
            ret_val.emplace_back(method_a(state));
        }
        ret_val.emplace_back(method_b(state));
        ret_val.emplace_back(method_c(state));
    }

    return ret_val;
}

bool check_for_wins(const std::vector<state_t>& states)
{
    for (const auto& state : states) {
        if (state.money > BREAK_THRES) {
            return true;
        }
    }

    return false;
}

std::string format_with_thousands_sep(long n)
{
    std::string s = std::to_string(n);
    int pos = s.length() - 3;
    while (pos > 0) {
        s.insert(pos, ",");
        pos -= 3;
    }
    return s;
}

int main()
{
    auto start = method_a(state_t{});

    std::vector<state_t> current_states;
    current_states.push_back(start);

    std::vector<state_t> winning_states;

    int current_iter = 0;
    int best_winning_time = 999999;

    while (true) {

        current_states = do_iter(current_states);

        // Copy out any winning states.
        std::copy_if(current_states.begin(), current_states.end(),
            std::back_inserter(winning_states),
            [](const state_t& state) {return state.money > BREAK_THRES;});

        // Print top wining states
        if (winning_states.size() != 0) {
            std::cout << "wining states" << std::endl;

            // Sort by lowest time
            std::sort(winning_states.begin(), winning_states.end(),
                [](state_t a, state_t b) {
                    return a.time_seconds < b.time_seconds;
                });

            int i = 0;
            for (const auto& state : winning_states) {
                print_state(state);
                if (++i > 4) { break; }
            }

            // Clear an save only the top
            auto top = winning_states.at(0);
            best_winning_time = top.time_seconds;
            winning_states.clear();
            winning_states.push_back(top);
        }

        // Delete if the state won, or if it's already taken longer than the current best win.
        std::remove_if(current_states.begin(), current_states.end(),
            [best_winning_time](const state_t& state) {
                return state.money > BREAK_THRES ||
                    state.time_seconds > best_winning_time;
            });

        // Sort states by rate
        std::sort(current_states.begin(), current_states.end(),
            [](state_t a, state_t b) {
                auto a_rate = a.money / a.time_seconds;
                auto b_rate = b.money / b.time_seconds;
                return a_rate > b_rate;
            });

        // Prune if there are too many
        if (current_states.size() > PRUNE_THRESHOLD) {
            std::cout << "PRUNED" << std::endl;
            std::vector<state_t> ret_val;
            ret_val.assign(current_states.begin(), current_states.begin() + PRUNE_THRESHOLD);
            current_states = ret_val;
        }

        std::cout << "===== Current iter: " << current_iter << " size: " << current_states.size() << std::endl;
        if (winning_states.size() != 0) {
            std::cout << "top wining state: ";
            print_state(winning_states.at(0));
        }

        // Print top states
        for (int i = 0; i < 3; ++i) {
            print_state(current_states.at(i));
        }


        ++current_iter;
    }

    // // Filter only the states that have won
    // std::vector<state_t> winning_states;

    // std::copy_if(current_states.begin(), current_states.end(),
    //     std::back_inserter(winning_states),
    //     [](const state_t& state) {return state.money > BREAK_THRES;});

    // // Sort by lowest time
    // std::sort(winning_states.begin(), winning_states.end(),
    //     [](state_t a, state_t b) {
    //         return a.time_seconds < b.time_seconds;
    //     });


    // std::cout << "Top 5:" << std::endl;
    // for (int i = 0; i < 5; ++i) {
    //     print_state(winning_states.at(i));
    // }

    std::cout << "DONE!" << std::endl;
}
