#include <algorithm>
#include <functional>
#include <iostream>
#include <locale>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class L1CFL {
 public:
  class GrammarError : public std::runtime_error {
   public:
    GrammarError(const std::string& message) : std::runtime_error(message) {}
  };

 private:
  const inline static char cStartMark = '@';
  const inline static char cEndMark = '$';

  const inline static std::function<void(size_t&, size_t)> combiner =
      [](size_t& h, size_t new_hash) {
        h ^= new_hash + 0x9e3779b9 + (h << 6) + (h >> 2);
      };

  // Action struct for table and whole algorithm
  enum class ActionType { ACCEPT, ERROR, SHIFT, REDUCE };
  struct Action {
    ActionType type;
    int value;
    bool operator==(const Action& another) const {
      return type == another.type && value == another.value;
    }
  };

  // Struct for reduce rule
  struct ReduceRule {
    int index;
    char lhs;
    std::string rhs;

    bool operator==(const ReduceRule& another) const {
      return index == another.index && lhs == another.lhs && rhs == another.rhs;
    }
  };

  struct ReduceRuleHash {
    std::size_t operator()(const ReduceRule& product) const {
      size_t total_hash = 0;
      combiner(total_hash, std::hash<int>{}(product.index));
      combiner(total_hash, std::hash<char>{}(product.lhs));
      combiner(total_hash, std::hash<std::string>{}(product.rhs));
      return total_hash;
    }
  };

  // LR(1) Item - state of parsing a word.
  struct LR1_Item {
    const ReduceRule& production;
    int dot;
    char lookahead;

    bool operator==(const LR1_Item& another) const {
      return production == another.production && dot == another.dot &&
             lookahead == another.lookahead;
    }
  };

  struct ItemHash {
    std::size_t operator()(const LR1_Item& item) const {
      size_t total_hash = 0;
      ReduceRuleHash hasher;
      combiner(total_hash, hasher(item.production));
      combiner(total_hash, std::hash<int>{}(item.dot));
      combiner(total_hash, std::hash<char>{}(item.lookahead));
      return total_hash;
    }
  };

  using ItemSet = std::unordered_set<LR1_Item, ItemHash>;

  struct ItemSetHash {
    size_t operator()(const ItemSet& item_set) const {
      size_t total_hash = 0;
      ItemHash hasher;
      for (const LR1_Item& item : item_set) {
        combiner(total_hash, hasher(item));
      }
      return total_hash;
    }
  };

  // Grammar Block
  std::unordered_set<char> alphabet_;
  std::unordered_set<char> non_terminals_;
  char start_non_terminal_;
  std::unordered_map<char, std::unordered_set<std::string>> rules_;

  // LR(1) Block Table Generation
  std::vector<ReduceRule> numbered_rules_;
  std::unordered_map<char, std::unordered_set<char>> first_sets_;
  std::vector<ItemSet> canonical_collection_;
  std::unordered_map<ItemSet, int, ItemSetHash> state_map_;
  std::unordered_set<char> all_symbols_;

  // Table Part
  std::unordered_map<int, std::unordered_map<char, Action>> action_table_;
  std::unordered_map<int, std::unordered_map<char, int>> goto_table_;
  bool table_built_ = false;

  void check_symbol_validity(char c, bool is_terminal_expected) const {
    if (c == cStartMark || c == cEndMark) {
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

  // LR Block
  void compute_first_sets() {
    for (char terminal : alphabet_) {
      first_sets_[terminal].insert(terminal);
    }
    first_sets_[cEndMark].insert(cEndMark);
    for (char non_terminal : non_terminals_) {
      first_sets_[non_terminal] = {};
    }

    bool changed = true;
    while (changed) {
      changed = false;
      for (char non_terminal : non_terminals_) {
        if (!rules_.contains(non_terminal)) {
          continue;
        }
        for (const std::string& rule : rules_.at(non_terminal)) {
          std::unordered_set<char> rhs_first;
          bool all_can_be_epsilon = true;
          if (rule == EPSILON) {
            rhs_first.insert(0);
          } else {
            for (char symbol : rule) {
              const std::unordered_set<char>& symbol_first =
                  first_sets_[symbol];
              bool has_epsilon = false;

              for (char first_sym : symbol_first) {
                if (first_sym == 0) {
                  has_epsilon = true;
                  continue;
                } else {
                  rhs_first.insert(first_sym);
                }
              }

              if (!has_epsilon) {
                all_can_be_epsilon = false;
                break;
              }
            }
          }

          if (all_can_be_epsilon) {
            rhs_first.insert(0);
          }
          size_t old_size = first_sets_[non_terminal].size();
          first_sets_[non_terminal].insert(rhs_first.begin(), rhs_first.end());
          if (first_sets_[non_terminal].size() > old_size) {
            changed = true;
          }
        }
      }
    }
  }

  std::unordered_set<char> get_first_for_string(std::string&& s,
                                                char look_forward) {
    std::unordered_set<char> result;
    bool all_have_epsilon = true;

    for (char symbol : s) {
      const std::unordered_set<char>& symbol_first = first_sets_[symbol];
      bool has_epsilon = false;

      for (char first_sym : symbol_first) {
        if (first_sym == 0) {
          has_epsilon = true;
        } else {
          result.insert(first_sym);
        }
      }
      if (!has_epsilon) {
        all_have_epsilon = false;
        break;
      }
    }
    if (all_have_epsilon) {
      result.insert(look_forward);
    }
    return result;
  }

  ItemSet closure(ItemSet&& item_set) {
    std::vector<LR1_Item> work_list(item_set.begin(), item_set.end());
    std::unordered_set<LR1_Item, ItemHash> processed;

    while (!work_list.empty()) {
      LR1_Item item = work_list.back();
      work_list.pop_back();

      if (processed.contains(item)) {
        continue;
      }
      processed.insert(item);
      if (item.dot >= item.production.rhs.size()) {
        continue;
      }

      char expected_not_terminal = item.production.rhs[item.dot];
      if (non_terminals_.contains(expected_not_terminal)) {
        std::unordered_set<char> lookaheads = get_first_for_string(
            item.production.rhs.substr(item.dot + 1), item.lookahead);
        for (const auto& prod : numbered_rules_) {
          if (prod.lhs == expected_not_terminal) {
            for (char b : lookaheads) {
              LR1_Item new_item = {prod, 0, b};
              if (!item_set.contains(new_item)) {
                item_set.insert(new_item);
                work_list.emplace_back(std::move(new_item));
              }
            }
          }
        }
      }
    }
    return item_set;
  }

  ItemSet goto_set(const ItemSet& item_set, char transition_symbol) {
    ItemSet target_kernel;
    for (const auto& item : item_set) {
      if (item.dot < item.production.rhs.size() &&
          item.production.rhs[item.dot] == transition_symbol) {
        target_kernel.emplace(
            LR1_Item{item.production, item.dot + 1, item.lookahead});
      }
    }
    return closure(std::move(target_kernel));
  }

  struct ParseActionRecord {
    ActionType type;
    int value;
    char symbol;
  };

  struct TreeNode {
    std::string label;
    std::vector<TreeNode*> children;

    TreeNode(char l) : label(std::string(1, l)) {}

    TreeNode(char l, std::vector<TreeNode*> c)
        : label(std::string(1, l)), children(std::move(c)) {
      std::reverse(children.begin(), children.end());
    }

    ~TreeNode() {
      for (TreeNode* child : children) {
        delete child;
      }
    }
  };

  static void print_node(const TreeNode* node, int depth,
                         std::ostream& os = std::cout) {
    if (node == nullptr) {
      return;
    }
    os << std::string(depth * 3, ' ');

    os << node->label;
    if (!node->children.empty()) {
      os << " -> ";
      for (size_t i = 0; i < node->children.size(); ++i) {
        os << node->children[i]->label
           << (i == node->children.size() - 1 ? "" : " ");
      }
    }
    os << "\n";

    for (TreeNode* child : node->children) {
      print_node(child, depth + 1, os);
    }
  }

 public:
  inline static const std::string EPSILON = "";

  struct Verdict {
    bool status;
    std::string info;
    std::vector<ParseActionRecord> action_history;
    const std::vector<ReduceRule>* numbered_rules_ptr;

    void print_parse_tree(std::ostream& os = std::cout) const {
      if (!status) {
        os << "ERROR: Cannot build parse tree. " << info << "\n";
        return;
      }

      std::stack<TreeNode*> node_stack;

      for (const auto& record : action_history) {
        if (record.type == ActionType::SHIFT) {
          node_stack.push(new TreeNode(record.symbol));
        } else if (record.type == ActionType::REDUCE) {
          int rule_index = record.value;
          const ReduceRule& rule = numbered_rules_ptr->at(rule_index);

          size_t rhs_len = rule.rhs.empty() ? 0 : rule.rhs.size();
          std::vector<TreeNode*> children;

          for (size_t i = 0; i < rhs_len; ++i) {
            if (node_stack.empty()) {
              os << "FATAL ERROR: Stack underflow during tree "
                    "reconstruction.\n";
              return;
            }
            children.push_back(node_stack.top());
            node_stack.pop();
          }
          if (rhs_len == 0 && rule.rhs == L1CFL::EPSILON) {
            children.push_back(new TreeNode('0'));
          }
          TreeNode* parent = new TreeNode(rule.lhs, std::move(children));

          node_stack.push(parent);
        }
      }

      if (node_stack.size() == 1) {
        const TreeNode* root = node_stack.top();

        os << "--- Reconstructed Parse Tree (Start from " << root->label
           << ") ---\n";
        if (!root->children.empty()) {
          L1CFL::print_node(root->children[0], 0, os);
        }
        os << "--------------------------------\n";
        delete node_stack.top();
      } else {
        os << "FATAL ERROR: Tree reconstruction failed (Stack size: "
           << node_stack.size() << ").\n";
        while (!node_stack.empty()) {
          delete node_stack.top();
          node_stack.pop();
        }
      }
    }
  };

  L1CFL(std::initializer_list<char> alph, std::initializer_list<char> non_term,
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
    all_symbols_.insert(alphabet_.begin(), alphabet_.end());
    all_symbols_.insert(non_terminals_.begin(), non_terminals_.end());
  }

  bool build_parsing_table(std::ostream& os = std::cout) {
    if (table_built_) {
      return table_built_;
    }
    non_terminals_.insert(cStartMark);
    alphabet_.insert(cEndMark);
    all_symbols_.insert(cStartMark);
    all_symbols_.insert(cEndMark);

    ReduceRule start_prod = {0, cStartMark,
                             std::string(1, start_non_terminal_)};
    numbered_rules_.push_back(start_prod);

    int rule_index = 1;
    for (const auto& pair : rules_) {
      char lhs = pair.first;
      for (const std::string& rhs : pair.second) {
        numbered_rules_.push_back({rule_index++, lhs, rhs});
      }
    }
    compute_first_sets();
    ItemSet initial_state = closure({{start_prod, 0, cEndMark}});
    state_map_[initial_state] = 0;
    canonical_collection_.emplace_back(std::move(initial_state));
    for (size_t current_state_id = 0;
         current_state_id < canonical_collection_.size(); ++current_state_id) {
      ItemSet current_state = canonical_collection_[current_state_id];
      for (char transition_symbol : all_symbols_) {
        ItemSet next_ker = goto_set(current_state, transition_symbol);
        if (next_ker.empty()) {
          continue;
        }
        int next_state_id;
        if (!state_map_.contains(next_ker)) {
          next_state_id = static_cast<int>(canonical_collection_.size());
          state_map_[next_ker] = next_state_id;
          canonical_collection_.emplace_back(std::move(next_ker));

        } else {
          next_state_id = state_map_[next_ker];
        }
        if (non_terminals_.contains(transition_symbol)) {
          goto_table_[current_state_id][transition_symbol] = next_state_id;
        } else {
          action_table_[current_state_id][transition_symbol] = {
              ActionType::SHIFT, next_state_id};
        }
      }
    }
    for (size_t i = 0; i < canonical_collection_.size(); ++i) {
      const ItemSet& current_state = canonical_collection_[i];
      for (const auto& item : current_state) {
        if (item.dot == static_cast<int>(item.production.rhs.size())) {
          Action new_action;
          if (cStartMark == item.production.lhs) {
            new_action = {ActionType::ACCEPT, 0};
          } else {
            new_action = {ActionType::REDUCE, item.production.index};
          }
          if (action_table_[i].contains(item.lookahead)) {
            os << "Grammar is not LR(1)" << '\n';
            return false;
          }
          action_table_[i][item.lookahead] = new_action;
        }
      }
    }
    table_built_ = true;
    return true;
  }

  void print_grammar(std::ostream& os = std::cout) const {
    std::set<char> printer;
    printer.insert(alphabet_.begin(), alphabet_.end());
    os << "Alphabet: ";
    for (char c : printer) {
      os << c << " ";
    }
    printer.clear();
    os << '\n';
    printer.insert(non_terminals_.begin(), non_terminals_.end());
    os << "Non terminals: ";
    for (char c : printer) {
      os << c << " ";
    }
    os << '\n';
    os << "Starting non terminal: " << start_non_terminal_ << '\n';
    os << "Rules:" << '\n';
    for (char c : printer) {
      for (std::string str : rules_.at(c)) {
        os << c << " -> " << (str == EPSILON ? "0" : str) << '\n';
      }
    }
  }

  void print_parsing_table(std::ostream& os = std::cout) const {
    if (!table_built_) {
      os << "Table is not ready!" << '\n';
      return;
    }

    os << "--- LR(1) Table ---" << '\n';

    std::vector<char> terminals(alphabet_.begin(), alphabet_.end());
    std::sort(terminals.begin(), terminals.end());
    std::vector<char> non_terminals_sorted;
    for (char c : non_terminals_) {
      if (c != cStartMark) {
        non_terminals_sorted.push_back(c);
      }
    }
    std::sort(non_terminals_sorted.begin(), non_terminals_sorted.end());

    os << "State |";
    for (char terminal : terminals) {
      os << "  " << terminal << "   |";
    }
    os << "|";
    for (char not_terminal : non_terminals_sorted) {
      os << "  " << not_terminal << "  |";
    }
    os << '\n';
    os << std::string((terminals.size() + non_terminals_sorted.size()) * 6 + 8,
                      '-')
       << '\n';

    for (size_t i = 0; i < canonical_collection_.size(); ++i) {
      os << "  " << i << "   |";
      for (char t : terminals) {
        if (action_table_.contains(i) && action_table_.at(i).contains(t)) {
          std::string s;
          switch (action_table_.at(i).at(t).type) {
            case ActionType::SHIFT:
              s = "S" + std::to_string(action_table_.at(i).at(t).value);
              break;
            case ActionType::REDUCE:
              s = "R" + std::to_string(action_table_.at(i).at(t).value);
              break;
            case ActionType::ACCEPT:
              s = "ACC";
              break;
            default:
              s = "ERR";
          }
          os << " " << s;
          os << std::string(5 - s.size(), ' ') << "|";
          continue;
        }
        os << "      |";
      }
      os << "|";
      for (char nt : non_terminals_sorted) {
        if (goto_table_.contains(i) && goto_table_.at(i).contains(nt)) {
          std::string s = std::to_string(goto_table_.at(i).at(nt));
          os << " " << s;
          os << std::string(4 - s.size(), ' ') << "|";
          continue;
        }
        os << "     |";
      }
      os << '\n';
    }
    os << "-------------------------------" << '\n';
    os << "REDUCE RULES:" << '\n';
    for (const auto& rule : numbered_rules_) {
      if (rule.index > 0) {
        os << "  ";
        os << "R" << rule.index << ": " << rule.lhs << " -> "
           << (rule.rhs == EPSILON ? "0" : rule.rhs) << '\n';
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
        continue;
      }
      insert_rule(non_term, rule);
    }
  }
  
  using ReduceRule = L1CFL::ReduceRule;
  Verdict parse(const std::string& input_string) const {
    std::vector<ParseActionRecord> history;
    if (!table_built_) {
      return {false, "Table is not ready", history, &numbered_rules_};
    }

    std::string input = input_string + cEndMark;
    int ip = 0;

    std::stack<int> state_stack;
    state_stack.push(0);

    while (true) {
      int s = state_stack.top();
      char a = input[ip];

      if (!action_table_.contains(s) || !action_table_.at(s).contains(a)) {
        std::string error_msg;
        if (a == cEndMark) {
          error_msg = "Reached end of input ('" + std::string(1, cEndMark) +
                      "') in state " + std::to_string(s) +
                      ". Input is a valid prefix but not a complete word.";
        } else {
          error_msg = "Unexpected symbol '" + std::string(1, a) +
                      "' at index " + std::to_string(ip) + " in state " +
                      std::to_string(s) + ".";
        }
        return {false, error_msg, history, &numbered_rules_};
      }
      Action action = action_table_.at(s).at(a);

      switch (action.type) {
        case ActionType::SHIFT: {
          history.push_back({ActionType::SHIFT, action.value, a});
          state_stack.push(action.value);
          ++ip;
          break;
        }
        case ActionType::REDUCE: {
          const ReduceRule& rule = numbered_rules_[action.value];

          history.push_back({ActionType::REDUCE, action.value, '\0'});

          for (size_t i = 0; i < rule.rhs.size(); ++i) {
            state_stack.pop();
          }

          int s_prime = state_stack.top();
          char left_side = rule.lhs;

          if (!goto_table_.contains(s_prime) ||
              !goto_table_.at(s_prime).contains(left_side)) {
            std::string error_msg =
                "No goto after reducing rule R" + std::to_string(action.value) +
                " (" + left_side + " -> " +
                (rule.rhs.empty() ? "0" : rule.rhs) + ") from state " +
                std::to_string(s_prime) + ".";
            return {false, error_msg, history, &numbered_rules_};
          }
          state_stack.push(goto_table_.at(s_prime).at(left_side));
          break;
        }
        case ActionType::ACCEPT: {
          history.push_back({ActionType::REDUCE, 0, '\0'});
          return {true, "Success.", history, &numbered_rules_};
        }
        case ActionType::ERROR: {
          return {false,
                  "Parsing error explicitly defined in the action table for "
                  "state " +
                      std::to_string(s) + " and symbol " + std::string(1, a) +
                      ".",
                  history, &numbered_rules_};
        }
      }
    }
  }
};
