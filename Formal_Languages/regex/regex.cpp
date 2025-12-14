#include <iostream>
#include <string>
#include <stack>
#include <queue>
#include <unordered_set>

namespace regex_pattern_detail {
    const char Eps = '1';
    const char Concat = '.';
    const char Unify = '+';
    const char Kleene = '*';
    const std::unordered_set<char> Alphabet = { 'a', 'b', 'c' };

    using Residues = std::unordered_set<int>;

    struct regex_exception : public std::runtime_error {
    public:
        regex_exception(const std::string& message) : std::runtime_error("Error in regex: " + message) {}
    };

    int k_mod;
    int l_target;

    Residues calc_union(const Residues& first, const Residues& second) {
        Residues result = first;
        result.insert(second.begin(), second.end());
        return result;
    }

    Residues calc_concat(const Residues& first, const Residues& second) {
        Residues result;
        for (int i : first) {
            for (int j : second) {
                int new_rem = (i + j) % k_mod;
                result.insert(new_rem);
            }
        }
        return result;
    }

    Residues calc_kleene(const Residues& res) {
        Residues result;
        result.insert(res.begin(), res.end());
        result.insert(0);
        std::queue<int> q;
        for (int r : res) {
            q.push(r);
        }
        q.push(0);
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            for (int r : res) {
                int v = (u + r) % k_mod;
                if (!result.contains(v)) {
                    result.insert(v);
                    q.push(v);
                }
            }
        }
        return result;
    }

    Residues calc_regex_residues(const std::string& rpn) {
        std::stack<Residues> dp_stack;
        for (char c : rpn) {
            if (Alphabet.contains(c)) {
                Residues res;
                res.insert(1 % k_mod);
                dp_stack.push(std::move(res));
            }
            else if (c == Eps) {
                Residues res;
                res.insert(0);
                dp_stack.push(std::move(res));
            }
            else if (c == Concat) {
                if (dp_stack.size() < 2) {
                    throw regex_exception("Missing operand for concatenation (.).");
                }
                Residues second = std::move(dp_stack.top());
                dp_stack.pop();
                Residues first = std::move(dp_stack.top());
                dp_stack.pop();
                dp_stack.push(calc_concat(first, second));
            }
            else if (c == Unify) {
                if (dp_stack.size() < 2) {
                    throw regex_exception("Missing operand for union (+).");
                }
                Residues second = std::move(dp_stack.top());
                dp_stack.pop();
                Residues first = std::move(dp_stack.top());
                dp_stack.pop();
                dp_stack.push(calc_union(first, second));
            }
            else if (c == Kleene) {
                if (dp_stack.empty()) {
                    throw regex_exception("Missing operand for Kleene star (*).");
                }
                Residues res = std::move(dp_stack.top());
                dp_stack.pop();
                dp_stack.push(calc_kleene(res));
            }
            else if (c != ' ') {
                throw regex_exception("Unidentified character: " + std::string(1, c));
            }
        }

        if (dp_stack.size() != 1) {
            throw regex_exception("Wrong number of operands or operators at the end.");
        }
        return std::move(dp_stack.top());
    }
}

std::string regex_mod_pattern(const std::string& regex, int k, int l) {
    if (k <= 0) {
        throw regex_pattern_detail::regex_exception("Parameter k must be positive.");
    }
    if (l < 0 || l >= k) {
        throw regex_pattern_detail::regex_exception("Parameter l must be in [0, k).");
    }
    if (regex == "") {
        return "NO";
    }
    if (k == 1) {
        return "YES";
    }
    regex_pattern_detail::k_mod = k;
    regex_pattern_detail::l_target = l;

    try {
        regex_pattern_detail::Residues final_state = regex_pattern_detail::calc_regex_residues(regex);
        if (final_state.contains(regex_pattern_detail::l_target)) {
            return "YES";
        }
        else {
            return "NO";
        }

    }
    catch (const regex_pattern_detail::regex_exception& e) {
        throw e;
    }
    catch (const std::exception& e) {
        throw regex_pattern_detail::regex_exception("An unexpected error occurred: " + std::string(e.what()));
    }
}

int main() {
    std::string regex;
    int k;
    int l;

    while (true) {
        std::cout << "========================================" << "\n";
        std::cout << "Write regex (or 'exit' to exit): ";
        std::cin >> regex;
        if (regex == "exit") {
            break;
        }
        std::cout << "Input k: ";
        if (!(std::cin >> k)) {
            continue;
        }
        std::cout << "Input l: ";
        if (!(std::cin >> l)) {
            continue;
        }
        try {
            std::string result = regex_mod_pattern(regex, k, l);
            std::cout << "-> Verdict: " << result << "\n";
        }
        catch (const std::exception& e) {
            std::cout << "-> Error: " << e.what() << "\n";
        }
    }

    std::cout << "Exiting..." << '\n';
}