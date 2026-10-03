#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

class CFL {
 public:
  class GrammarError : public std::runtime_error {
   public:
    GrammarError(const std::string& message) : std::runtime_error(message) {}
  };

 private:
  std::unordered_set<char> alphabet_;
  std::unordered_set<char> non_terminals_;
  char start_non_terminal_;
  std::unordered_map<char, std::unordered_set<std::string>> rules_;

  void check_symbol_validity(char c, bool is_terminal_expected) const {
    if (c == '$') {
      throw GrammarError("Grammar error: Symbol '" + std::string(1, c) +
                         "' is used as technical one and forbidden for use.");
    }
    if (std::isalpha(c)) {
      if (is_terminal_expected) {
        if (std::isupper(c)) {
          throw GrammarError("Grammar error: Symbol '" + std::string(1, c) +
                             "' can't be terminal.");
        }
      } else {
        if (std::islower(c)) {
          throw GrammarError("Grammar error: Symbol '" + std::string(1, c) +
                             "' can't be non terminal.");
        }
      }
    }
  }

  // EARLEY BLOCK
  struct EarleyItem {
    char lhs;
    std::string rhs;
    int dot;
    int origin;
    bool operator==(const EarleyItem& another) const {
      return lhs == another.lhs && rhs == another.rhs && dot == another.dot &&
             origin == another.origin;
    }
  };

  struct EarleyItemHash {
    std::size_t operator()(const EarleyItem& item) const {
      std::size_t h1 = std::hash<char>{}(item.lhs);
      std::size_t h2 = std::hash<std::string>{}(item.rhs);
      std::size_t h3 = std::hash<int>{}(item.dot);
      std::size_t h4 = std::hash<int>{}(item.origin);
      return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
    }
  };

  void Scan(const std::string& word, const EarleyItem& scan_item, int pos,
            std::unordered_set<EarleyItem, EarleyItemHash>& D_next) const {
    if (pos < static_cast<int>(word.size()) &&
        scan_item.rhs[scan_item.dot] == word[pos]) {
      EarleyItem new_item = scan_item;
      ++new_item.dot;
      D_next.insert(std::move(new_item));
    }
  }

  void Predict(char non_terminal, int pos,
               std::unordered_set<EarleyItem, EarleyItemHash>& D_next) const {
    if (!rules_.contains(non_terminal)) {
      return;
    }
    for (const std::string& right_part : rules_.at(non_terminal)) {
      EarleyItem new_item{non_terminal, right_part, 0, pos};
      D_next.insert(std::move(new_item));
    }
  }

  void Complete(
      const EarleyItem& complete_item, int pos,
      std::vector<std::unordered_set<EarleyItem, EarleyItemHash>>& D) const {
    char complete_non_terminal = complete_item.lhs;
    const std::unordered_set<EarleyItem, EarleyItemHash>& D_origin =
        D[complete_item.origin];
    for (const EarleyItem& potential_origin : D_origin) {
      if (potential_origin.dot <
              static_cast<int>(potential_origin.rhs.size()) &&
          potential_origin.rhs != EPSILON) {
        if (complete_non_terminal ==
            potential_origin.rhs[potential_origin.dot]) {
          EarleyItem complete_item = potential_origin;
          ++complete_item.dot;
          std::unordered_set<EarleyItem, EarleyItemHash>& D_pos = D[pos];
          D_pos.insert(std::move(complete_item));
        }
      }
    }
  }

 public:
  inline static const std::string EPSILON = " ";

  CFL(std::initializer_list<char> alph, std::initializer_list<char> non_term,
      char s)
      : start_non_terminal_(s) {
    for (char c : alph) {
      check_symbol_validity(c, true);
      alphabet_.insert(c);
    }
    for (char c : non_term) {
      check_symbol_validity(c, false);
      if (alphabet_.contains(c)) {
        throw GrammarError("Grammar error: Symbol '" + std::string(1, c) +
                           "' can't be both terminal and non terminal.");
      }
      non_terminals_.insert(c);
    }
    if (!non_terminals_.contains(s)) {
      throw GrammarError("Grammar error: Symbol '" + std::string(1, s) +
                         "' must be in non terminals.");
    }
  }
  void print_grammar() const {
    std::set<char> printer;
    printer.insert(alphabet_.begin(), alphabet_.end());
    std::cout << "Alphabet: ";
    for (char c : printer) {
      std::cout << c << " ";
    }
    printer.clear();
    std::cout << '\n';
    printer.insert(non_terminals_.begin(), non_terminals_.end());
    std::cout << "Non terminals: ";
    for (char c : printer) {
      std::cout << c << " ";
    }
    std::cout << '\n';
    std::cout << "Starting non terminal: " << start_non_terminal_ << '\n';
    std::cout << "Rules:" << '\n';
    for (char c : printer) {
      for (std::string str : rules_.at(c)) {
        std::cout << c << " -> " << (str == EPSILON ? "0" : str) << '\n';
      }
    }
  }
  void insert_rule(char non_term, const std::string& to) {
    if (!non_terminals_.contains(non_term)) {
      throw GrammarError("Rule error: Left part: '" + std::string(1, non_term) +
                         "' is not defined non terminal.");
    }
    check_symbol_validity(non_term, false);
    if (to != EPSILON) {
      for (char c : to) {
        if (alphabet_.contains(c)) {
          check_symbol_validity(c, true);
        } else if (non_terminals_.contains(c)) {
          check_symbol_validity(c, false);
        } else {
          throw GrammarError("Rule error '" + std::string(1, non_term) +
                             " -> " + to + "': Symbol '" + std::string(1, c) +
                             "' is unknown");
        }
      }
    }
    rules_[non_term].insert(to);
  }
  void insert_rules(char non_term, const std::string& rules_string) {
    std::stringstream ss(rules_string);
    std::string rule;

    while (std::getline(ss, rule, '|')) {
      if (rule.empty()) {
        continue;
      }
      if (rule == "0") {
        insert_rule(non_term, EPSILON);
      } else {
        insert_rule(non_term, rule);
      }
    }
  }

  bool earley_check(const std::string& to_check) const {
    std::vector<std::unordered_set<EarleyItem, EarleyItemHash>> D(
        to_check.size() + 1);
    if (!rules_.contains(start_non_terminal_)) {
      return false;
    }
    for (const std::string& rule : rules_.at(start_non_terminal_)) {
      D[0].insert({start_non_terminal_, rule, 0, 0});
    }
    for (size_t k = 0; k <= to_check.size(); ++k) {
      size_t size = D[k].size();
      do {
        size = D[k].size();
        for (const EarleyItem& item : D[k]) {
          char next_sym = '$';
          if (item.dot < item.rhs.length() && item.rhs != EPSILON) {
            next_sym = item.rhs[item.dot];
          }
          if (next_sym == '$') {
            Complete(item, k, D);
          } else {
            Predict(next_sym, k, D[k]);
          }
        }
      } while (D[k].size() != size);
      if (k < to_check.size()) {
        for (const EarleyItem& scan_item : D[k]) {
          Scan(to_check, scan_item, k, D[k + 1]);
        }
      }
    }
    for (const auto& item : D[to_check.size()]) {
      if (item.lhs == start_non_terminal_ &&
          (item.dot == static_cast<int>(item.rhs.size()) ||
           item.rhs == EPSILON) &&
          item.origin == 0) {
        return true;
      }
    }
    return false;
  }
};
