#include <iostream>
#include <vector>
#include <cstdint>
#include <algorithm>

constexpr long BREAK_THRES = 1'000'000'000;
constexpr long PRUNE_THRESHOLD = 20000000;

// xxx top wining state: time: 3398 money: 1,029,000,000 money/time: 302825 aaaaacbbbbbbcbbbbbbcbbbbbbcbbbbbbbbbbbbbbbbbbbbbbb
// xxx top wining state: time: 3082 money: 1,017,450,000 money/time: 330126 aaaaaaacbbbbbbbbbbbbbbbbcbbbbbbbbbbbbbbbbbcbbbbbbbbbbbbbbbbbb

// xxx top wining state: time: 3350 money: 2,125,200,000 money/time: 634388 aaaaaaaacbbbbbbbbbbbbbbbbbbbbbcbbbbbbbbbbbbbbbbbbbbcbbbbbbbbbbbbbbbbbbbbbb

// xxx top wining state: time: 3560 money: 4,140,500,000 money/time: 1163061 aaaaacbbbbbbbbbbbbcbbbbbbbbbbbbcbbbbbbbbbbbbbcbbbbbbbbbbbbb

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

void print_state(const state_t& state)
{
    std::cout << "time: " << state.time_seconds << " money: " << format_with_thousands_sep(state.money) << " money/time: " << state.money / state.time_seconds << " ";
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
        // Assume that we only do a at the start, and never after saving
        // to reduce problem space
        if (!state.has_saved) {
            ret_val.emplace_back(method_a(state));
        }
        // No point in b if we haven't saved at least once
        if (state.has_saved) {
            ret_val.emplace_back(method_b(state));
        }
        // If we do a c, then always do a b after, there's no point in doing
        // multiple c's in a row.
        ret_val.emplace_back(method_b(method_c(state)));
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

int main()
{
    auto start = method_a(state_t{});

    std::vector<state_t> current_states;
    current_states.push_back(start);

    state_t best_winning_state = {};
    std::vector<state_t> all_winning_states;

    int current_iter = 0;

    while (true) {

        current_states = do_iter(current_states);

        std::vector<state_t> next_iter;
        int win_count = 0;
        int too_long_count = 0;

        // See if we can get rid of any
        for (const auto& state : current_states) {

            if (state.money > BREAK_THRES) {
                if (best_winning_state.money == 0) {
                    best_winning_state = state;
                }
                if (state.time_seconds < best_winning_state.time_seconds) {
                    best_winning_state = state;
                    all_winning_states.clear();
                    all_winning_states.push_back(best_winning_state);
                }
                if (state.time_seconds == best_winning_state.time_seconds) {
                    all_winning_states.push_back(state);
                }
                ++win_count;
                // else just drop the useless win
                continue;
            }

            if (best_winning_state.time_seconds != 0 &&
                state.time_seconds > best_winning_state.time_seconds) {
                ++too_long_count;
                // drop state that has already taken too long
                continue;
            }

            // any remaining get copied to next iter
            next_iter.push_back(state);
        }

        current_states = next_iter;

        // Sort states by rate
        std::sort(current_states.begin(), current_states.end(),
            [](state_t a, state_t b) {
                auto a_rate = a.money / a.time_seconds;
                auto b_rate = b.money / b.time_seconds;
                return a_rate > b_rate;
            });

        std::cout << "===== Current iter: " << current_iter << " size: " << format_with_thousands_sep(current_states.size())
            << " win count: " << win_count << " too long count: " << too_long_count << std::endl;
        if (best_winning_state.money != 0) {
            std::cout << "xxx top wining states: " << std::endl;
            print_all(all_winning_states);
        }

        if (current_states.size() >= 3) {
        // Print top states by rate
            for (int i = 0; i < 3; ++i) {
                print_state(current_states.at(i));
            }
            // And bottom states
            for (unsigned i = current_states.size() - 3; i < current_states.size(); ++i) {
                print_state(current_states.at(i));
            }
        }


        ++current_iter;

        if (current_states.size() == 0) { break; }
    }

    std::cout << "DONE!" << std::endl;
}
