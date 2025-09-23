#include <algorithm>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <string>

/*
My first approach to formal languages theory
Library includes NFA and DFA classes with operations:
1) E-elimination
2) NFA to DFA conversion (subset-method approach)
3) DFA to Complete DFA
4) DFA minimization (Hopcroft's algorithm approach)
5) DFA's complement
6) DFA to regex. (might be too large sometimes)
Notes: epsilon = E. NFA and DFA default-constructors are forbidden.
DFA is only a product of NFA::toDFA() method. We can't change DFA's structure.

TO DO:
0) Test coverage
1) Rewrite it using inheritance and polymorphism but it is complicated to some
degree. std::conditional will be useful.
2) Simplify regex algorithm. Regex may
be too complicated sometimes. But they are still equal to the true ones.
*/

template <typename Symbol = char, typename State = int>
class NFA;

//DFA CLASS
template <typename Symbol = char, typename State = int>
class DFA {
 private:
  //FIELDS
  friend NFA<Symbol, State>;
  std::set<Symbol> alphabet_;     //Σ
  std::set<State> states_;        // Q
  State start_state_ = 0;         // s∈Q
  std::set<State> final_states_;  // T⊂Q
  std::map<std::pair<State, Symbol>, State>
      transition_function_;  //δ:Q×Σ→Q |w|=1
  bool is_complete_ = false;

  //PRIVATE METHODS
  void add_transition(State from, Symbol sym, State to) {
    transition_function_[{from, sym}] = to;
  }

  bool is_final(State state) const { return final_states_.contains(state); }

  //REGEX
  std::string regex_union(const std::string& a, const std::string& b) const {
    if (a.empty()) {
      return b;
    }
    if (b.empty()) {
      return a;
    }
    if (a == b) {
      return a;
    }
    return "(" + a + "|" + b + ")";
  }

  std::string regex_concat(const std::string& a, const std::string& b) const {
    if (a.empty() || b.empty()) {
      return "";
    }
    if (a == "E") {
      return b;
    }
    if (b == "E") {
      return a;
    }
    return a + b;
  }

  std::string regex_kleene(const std::string& s) const {
    if (s.empty() || s == "E") {
      return "E";
    }
    return "(" + s + ")*";
  }
  DFA() = default;

 public:
  //PRINTER
  void print() {
    std::cout << "-----------------------------" << '\n';
    std::cout << "Alphabet: ";
    for (Symbol letter : alphabet_) {
      std::cout << letter << " ";
    }
    std::cout << '\n';
    std::cout << "States: ";
    for (State st : states_) {
      std::cout << st << " ";
    }
    std::cout << '\n';
    std::cout << "Start state: " << start_state_ << '\n';
    std::cout << "Final states: ";
    for (State st : final_states_) {
      std::cout << st << " ";
    }
    std::cout << '\n';
    std::cout << "Transitions: " << '\n';
    for (State from : states_) {
      for (Symbol letter : alphabet_) {
        if (transition_function_.contains({from, letter})) {
          std::cout << "  (" << from << ", " << letter << ") -> "
                    << transition_function_[{from, letter}] << '\n';
        }
      }
    }
    std::cout << "-----------------------------" << '\n';
  }

  //COMPLETE
  void complete() {
    bool need_sink = false;
    for (State state : states_) {
      for (Symbol letter : alphabet_) {
        if (!transition_function_.contains({state, letter})) {
          need_sink = true;
          break;
        }
      }
    }
    if (need_sink) {
      State sink_state = *states_.rbegin() + 1;
      states_.insert(sink_state);
      for (Symbol letter : alphabet_) {
        add_transition(sink_state, letter, sink_state);
      }
      for (State state : states_) {
        for (Symbol letter : alphabet_) {
          if (!transition_function_.contains({state, letter})) {
            add_transition(state, letter, sink_state);
          }
        }
      }
    }
    is_complete_ = true;
    return;
  }

  DFA completed() const {
    DFA copy = *this;
    copy.complete();
    return copy;
  }

  //INVERT
  void invert() {
    if (!is_complete_) {
      complete();
    }
    std::set<State> new_final_states = states_;
    for (State st : final_states_) {
      if (new_final_states.contains(st)) {
        new_final_states.erase(st);
      }
    }
    final_states_ = new_final_states;
  }

  DFA inverted() const {
    DFA copy = *this;
    copy.invert();
    return copy;
  }

  //MINIMIZE (by Hopcroft)
  void minimize() {
    std::set<State> reachable_states;
    std::queue<State> BFS_queue;
    BFS_queue.push(start_state_);
    reachable_states.insert(start_state_);

    while (!BFS_queue.empty()) {
      State current = BFS_queue.front();
      BFS_queue.pop();
      for (Symbol letter : alphabet_) {
        if (transition_function_.contains({current, letter})) {
          State next = transition_function_.at({current, letter});
          if (!reachable_states.contains(next)) {
            reachable_states.insert(next);
            BFS_queue.push(next);
          }
        }
      }
    }
    DFA temp_DFA;
    temp_DFA.alphabet_ = alphabet_;
    temp_DFA.start_state_ = start_state_;
    for (State st : reachable_states) {
      temp_DFA.states_.insert(st);
      if (final_states_.contains(st)) {
        temp_DFA.final_states_.insert(st);
      }
      for (Symbol letter : alphabet_) {
        if (transition_function_.contains({st, letter}) &&
            reachable_states.contains(transition_function_.at({st, letter}))) {
          temp_DFA.add_transition(st, letter,
                                  transition_function_.at({st, letter}));
        }
      }
    }
    std::set<State> final_states;
    std::set<State> non_final_states;
    std::set<std::set<State>> partition;
    for (State st : temp_DFA.states_) {
      if (is_final(st)) {
        final_states.insert(st);
      } else {
        non_final_states.insert(st);
      }
    }
    if (!final_states.empty()) {
      partition.insert(final_states);
    }
    if (!non_final_states.empty()) {
      partition.insert(non_final_states);
    }
    std::queue<std::set<State>> worklist;
    if (final_states.size() > non_final_states.size()) {
      if (!non_final_states.empty()) {
        worklist.push(non_final_states);
      }
    } else {
      if (!final_states.empty()) {
        worklist.push(final_states);
      }
    }
    while (!worklist.empty()) {
      std::set<State> first = worklist.front();
      worklist.pop();

      for (Symbol letter : temp_DFA.alphabet_) {
        std::set<State> x;
        for (State state : temp_DFA.states_) {
          if (temp_DFA.transition_function_.contains({state, letter})) {
            if (first.contains(
                    temp_DFA.transition_function_.at({state, letter}))) {
              x.insert(state);
            }
          }
        }
        std::set<std::set<State>> new_partition;
        bool changed = false;
        for (const auto& y : partition) {
          std::set<State> y_intersect_x;
          std::set_intersection(
              y.begin(), y.end(), x.begin(), x.end(),
              std::inserter(y_intersect_x, y_intersect_x.begin()));

          std::set<State> y_minus_x;
          std::set_difference(y.begin(), y.end(), x.begin(), x.end(),
                              std::inserter(y_minus_x, y_minus_x.begin()));

          if (!y_intersect_x.empty() && !y_minus_x.empty()) {
            new_partition.insert(y_intersect_x);
            new_partition.insert(y_minus_x);

            if (worklist.empty() ||
                worklist.front().size() > y_intersect_x.size()) {
              worklist.push(y_intersect_x);
            } else {
              worklist.push(y_minus_x);
            }
            changed = true;
          } else {
            new_partition.insert(y);
          }
        }

        if (changed) {
          partition = new_partition;
          std::queue<std::set<State>> next_worklist;
          std::set<std::set<State>> new_partitions_in_worklist;
          while (!worklist.empty()) {
            std::set<State> current = worklist.front();
            worklist.pop();
            bool found = false;
            for (const auto& p : new_partition) {
              if (p.size() == current.size() &&
                  std::equal(p.begin(), p.end(), current.begin())) {
                found = true;
                if (!new_partitions_in_worklist.contains(p)) {
                  next_worklist.push(p);
                  new_partitions_in_worklist.insert(p);
                }
                break;
              }
            }
            if (!found) {
              std::set<State> new_y1;
              std::set<State> new_y2;
              std::set_intersection(current.begin(), current.end(), x.begin(),
                                    x.end(),
                                    std::inserter(new_y1, new_y1.begin()));
              std::set_difference(current.begin(), current.end(), x.begin(),
                                  x.end(),
                                  std::inserter(new_y2, new_y2.begin()));
              if (new_y1.size() > new_y2.size()) {
                if (!new_partitions_in_worklist.contains(new_y2)) {
                  next_worklist.push(new_y2);
                  new_partitions_in_worklist.insert(new_y2);
                }

              } else {
                if (!new_partitions_in_worklist.contains(new_y1)) {
                  next_worklist.push(new_y1);
                  new_partitions_in_worklist.insert(new_y1);
                }
              }
            }
          }
          worklist = next_worklist;
        }
      }
    }

    DFA min_DFA;
    min_DFA.alphabet_ = temp_DFA.alphabet_;
    std::map<State, State> old_state_to_new_state;
    State new_state_counter = 0;

    for (const auto& block : partition) {
      State representative = *block.begin();
      State newState = new_state_counter++;
      min_DFA.states_.insert(newState);
      for (State s : block) {
        old_state_to_new_state[s] = newState;
      }
      if (is_final(representative)) {
        min_DFA.final_states_.insert(newState);
      }
    }
    min_DFA.start_state_ = old_state_to_new_state[temp_DFA.start_state_];

    for (const auto& block : partition) {
      State representative = *block.begin();
      for (Symbol letter : min_DFA.alphabet_) {
        if (temp_DFA.transition_function_.contains({representative, letter})) {
          State old_next_state =
              temp_DFA.transition_function_.at({representative, letter});
          State new_next_state = old_state_to_new_state[old_next_state];
          min_DFA.add_transition(old_state_to_new_state[representative], letter,
                                 new_next_state);
        }
      }
    }
    *this = min_DFA;
  }

  void minimized() const {
    DFA copy = *this;
    copy.minimize();
    return copy;
  }

  //REGEX
  std::string to_regex() const {
    if (final_states_.empty()) {
      return "[empty set]";
    }
    std::set<State> states = states_;
    State start = start_state_;
    std::set<State> finals = final_states_;
    std::map<std::pair<State, Symbol>, State> transitions =
        transition_function_;
    State new_start = *std::max_element(states.begin(), states.end()) + 1;
    State new_final = new_start + 1;
    states.insert(new_start);
    states.insert(new_final);
    std::map<State, std::map<State, std::string>> regex_matrix;
    for (State i : states) {
      for (State j : states) {
        regex_matrix[i][j] = "";
      }
    }
    for (const auto& trans : transitions) {
      State from = trans.first.first;
      Symbol sym = trans.first.second;
      State to = trans.second;
      std::string sym_str = (sym == '\0') ? "E" : std::string(1, sym);
      regex_matrix[from][to] = regex_union(regex_matrix[from][to], sym_str);
    }
    regex_matrix[new_start][start] =
        regex_union(regex_matrix[new_start][start], "E");
    for (State f : finals) {
      regex_matrix[f][new_final] = regex_union(regex_matrix[f][new_final], "E");
    }
    if (finals.contains(start)) {
      regex_matrix[new_start][new_final] =
          regex_union(regex_matrix[new_start][new_final], "E");
    }
    for (State k : states_) {
      for (State i : states) {
        if (i == k) {
          continue;
        }
        for (State j : states) {
          if (j == k) {
            continue;
          }
          std::string matrix_ik = regex_matrix[i][k];
          std::string matrix_kk = regex_matrix[k][k];
          std::string matrix_kj = regex_matrix[k][j];

          std::string kleene_kk = regex_kleene(matrix_kk);
          std::string part = regex_concat(matrix_ik, kleene_kk);
          std::string new_expr = regex_concat(part, matrix_kj);

          if (!new_expr.empty()) {
            regex_matrix[i][j] = regex_union(regex_matrix[i][j], new_expr);
          }
        }
      }
    }
    return regex_matrix[new_start][new_final];
  }
};

//NFA CLASS
template <typename Symbol = char, typename State = int>
class NFA {
 private:
  //FIELDS
  std::set<Symbol> alphabet_;     //Σ
  std::set<State> states_;        // Q
  State start_state_ = 0;         // s∈Q
  std::set<State> final_states_;  // T⊂Q
  std::map<std::pair<State, Symbol>, std::set<State>>
      transition_function_;  //δ:Q×Σ→Q |w|=1
  //CONSTRUCTOR (private)
  NFA() = default;

 public:
  NFA(const std::set<Symbol>& syms, const std::set<State>& states, State start,
      const std::set<State>& finals)
      : alphabet_(syms),
        states_(states),
        start_state_(start),
        final_states_(finals) {}

  //TRANSITION ADD/DELETE
  void add_transition(State from, Symbol sym, State to) {
    transition_function_[{from, sym}].insert(to);
  }

  void delete_transition(State from, Symbol sym, State to) {
    transition_function_[{from, sym}].erase(to);
  }

  //PRINTER
  void print() {
    std::cout << "-----------------------------" << '\n';
    std::cout << "Alphabet: ";
    for (Symbol letter : alphabet_) {
      if (letter != '\0') {
        std::cout << letter << " ";
        continue;
      }
      std::cout << "E"
                << " ";
    }
    std::cout << '\n';
    std::cout << "States: ";
    for (State st : states_) {
      std::cout << st << " ";
    }
    std::cout << '\n';
    std::cout << "Start state: " << start_state_ << '\n';
    std::cout << "Final states: ";
    for (State st : final_states_) {
      std::cout << st << " ";
    }
    std::cout << '\n';
    std::cout << "Transitions: " << '\n';
    for (State from : states_) {
      for (Symbol letter : alphabet_) {
        if (transition_function_.contains({from, letter})) {
          for (State in : transition_function_[{from, letter}]) {
            std::cout << "  (" << from << ", "
                      << (letter != '\0' ? letter : 'E') << ") -> " << in
                      << '\n';
          }
        }
      }
    }
    std::cout << "-----------------------------" << '\n';
  }

  //EPSILON ELIMINATION BLOCK
  std::set<State> get_epsilon_closure(State state) const {
    std::set<State> closure;
    std::queue<State> BFS_queue;
    BFS_queue.push(state);
    closure.insert(state);
    while (!BFS_queue.empty()) {
      State current = BFS_queue.front();
      BFS_queue.pop();
      if (transition_function_.contains({current, '\0'})) {
        for (State next : transition_function_.at({current, '\0'})) {
          if (!closure.contains(next)) {
            closure.insert(next);
            BFS_queue.push(next);
          }
        }
      }
    }
    return closure;
  }

  void eliminate_epsilon_transitions() {
    NFA new_NFA;
    new_NFA.states_ = states_;
    new_NFA.alphabet_ = alphabet_;
    new_NFA.start_state_ = start_state_;
    for (State state : states_) {
      std::set<State> closure = get_epsilon_closure(state);
      for (Symbol symbol : alphabet_) {
        if (symbol == '\0') {
          continue;
        }
        std::set<State> next_states;
        for (State closed_state : closure) {
          if (transition_function_.contains({closed_state, symbol})) {
            for (State next : transition_function_.at({closed_state, symbol})) {
              std::set<State> next_closure = get_epsilon_closure(next);
              next_states.insert(next_closure.begin(), next_closure.end());
            }
          }
        }
        if (!next_states.empty()) {
          for (State s : next_states) {
            new_NFA.add_transition(state, symbol, s);
          }
        }
      }

      for (State closed_state : closure) {
        if (final_states_.contains(closed_state)) {
          new_NFA.final_states_.insert(state);
          break;
        }
      }
    }
    *this = new_NFA;
  }

  //THE ONLY WAY TO CREATE DFA
  DFA<Symbol, State> toDFA() const {
    DFA dfa;
    dfa.alphabet_ = alphabet_;
    dfa.alphabet_.erase('\0');
    std::map<std::set<State>, State> subsets;
    std::queue<std::set<State>> BFS_queue;
    State state_сounter = 0;
    std::set<State> start_subset = get_epsilon_closure(start_state_);
    BFS_queue.push(start_subset);
    subsets[start_subset] = state_сounter++;
    dfa.start_state_ = subsets[start_subset];
    dfa.states_.insert(dfa.start_state_);

    while (!BFS_queue.empty()) {
      std::set<State> current_subset = BFS_queue.front();
      BFS_queue.pop();
      State current_state = subsets[current_subset];
      for (State NFA_state : current_subset) {
        if (final_states_.contains(NFA_state)) {
          dfa.final_states_.insert(current_state);
          break;
        }
      }
      for (Symbol letter : dfa.alphabet_) {
        std::set<State> next_subset;
        for (State NFA_state : current_subset) {
          if (transition_function_.contains({NFA_state, letter})) {
            for (State s : transition_function_.at({NFA_state, letter})) {
              std::set<State> closure = get_epsilon_closure(s);
              next_subset.insert(closure.begin(), closure.end());
            }
          }
        }
        if (!next_subset.empty()) {
          if (!subsets.contains(next_subset)) {
            subsets[next_subset] = state_сounter++;
            BFS_queue.push(next_subset);
            dfa.states_.insert(subsets[next_subset]);
          }
          dfa.add_transition(current_state, letter, subsets[next_subset]);
        }
      }
    }
    return dfa;
  }
};
