#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "earley.h"
#include "gtest/gtest.h"

using namespace std;

void CheckEarley(const CFL& grammar,
                 const std::vector<std::string>& should_pass,
                 const std::vector<std::string>& should_fail) {
  for (const auto& s : should_pass) {
    bool status = grammar.earley_check(s);
    EXPECT_TRUE(status) << "Should ACCEPT: '" << s << "' but REJECTED.";
  }

  for (const auto& s : should_fail) {
    bool status = grammar.earley_check(s);
    EXPECT_FALSE(status) << "Should REJECT: '" << s << "' but ACCEPTED.";
  }
}

void generate_strings(int n, string current, vector<string>& result) {
  if (n == 0) {
    result.push_back(current);
    return;
  }
  generate_strings(n - 1, current + 'a', result);
  generate_strings(n - 1, current + 'b', result);
}

void split_strings_by_criterion(int max_len, vector<string>& should_pass,
                                vector<string>& should_fail) {
  for (int len = 0; len <= max_len; ++len) {
    vector<string> all_strings;
    generate_strings(len, "", all_strings);

    for (const auto& s : all_strings) {
      int Na = 0;
      int Nb = 0;
      for (char c : s) {
        if (c == 'a')
          Na++;
        else if (c == 'b')
          Nb++;
      }

      if (Na >= Nb && Na <= 2 * Nb) {
        should_pass.push_back(s);
      } else {
        should_fail.push_back(s);
      }
    }
  }
}

TEST(CFL_EarleyExceptionTest, InvalidSymbolsConstructor) {
  EXPECT_THROW(CFL({'$'}, {'S'}, 'S'), CFL::GrammarError);

  EXPECT_THROW(CFL({'a'}, {'S', 'a'}, 'S'), CFL::GrammarError);

  EXPECT_THROW(CFL({'a'}, {'A'}, 'S'), CFL::GrammarError);

  EXPECT_THROW(CFL({'A'}, {'S'}, 'S'), CFL::GrammarError);
  EXPECT_THROW(CFL({'a'}, {'s'}, 's'), CFL::GrammarError);
}

TEST(CFL_EarleyExceptionTest, InvalidRuleInsertion) {
  CFL gram({'a'}, {'S', 'A'}, 'S');

  EXPECT_THROW(gram.insert_rule('X', "a"), CFL::GrammarError);

  EXPECT_THROW(gram.insert_rule('S', "b"), CFL::GrammarError);

  EXPECT_THROW(gram.insert_rule('S', "A B"), CFL::GrammarError);
}

TEST(CFL_EarleyParsingTest, DyckLanguage) {
  CFL gram({'(', ')'}, {'S'}, 'S');
  gram.insert_rules('S', "0|(S)S");

  CheckEarley(gram, {"", "()", "(())", "()()", "()(())", "((()))(())"},
              {"(", ")", ")(", "(()", "(())("});
}

TEST(CFL_EarleyParsingTest, ArithmeticExpressions) {
  CFL gram({'i', '+', '*', '(', ')'}, {'E', 'T', 'F'}, 'E');
  gram.insert_rules('E', "E+T|T");
  gram.insert_rules('T', "T*F|F");
  gram.insert_rules('F', "(E)|i");

  CheckEarley(gram, {"i", "i+i", "i*i", "i+i*i", "(i+i)*i", "((i))"},
              {"", "+", "i+", "+i", "()", "i()i", "i++i"});
}

TEST(CFL_EarleyParsingTest, LR1_Not_SLR1_LValue) {
  CFL gram({'=', '*', 'i'}, {'S', 'L', 'R'}, 'S');
  gram.insert_rules('S', "L=R|R");
  gram.insert_rules('L', "*R|i");
  gram.insert_rules('R', "L");

  CheckEarley(gram, {"i", "*i", "**i", "i=i", "*i=i"}, {"=", "i=", "=i", "**"});
}

TEST(CFL_EarleyParsingTest, LeftRecursion_Works) {
  CFL gram({'a', 'b'}, {'S'}, 'S');
  gram.insert_rules('S', "Sa|b");

  CheckEarley(gram, {"b", "ba", "baa", "baaaaa"},
              {"", "a", "ab", "bba", "baaab"});
}

TEST(CFL_EarleyParsingTest, CyclicGrammar_Works) {
  CFL gram({'a'}, {'S'}, 'S');
  gram.insert_rules('S', "S|a");

  CheckEarley(gram, {"a"}, {"", "aa"});
}

TEST(CFL_EarleyParsingTest, Ambiguity_ShiftReduce) {
  CFL gram({'a', 'b'}, {'S'}, 'S');
  gram.insert_rules('S', "SaS|b");

  CheckEarley(gram, {"b", "bab", "babab", "bababab"},
              {"", "a", "aa", "bb", "babaa", "ab", "aababaa"});
}

TEST(CFL_EarleyParsingTest, Ambiguity_ReduceReduce) {
  CFL gram({'x'}, {'S', 'A', 'B'}, 'S');
  gram.insert_rules('S', "A|B");
  gram.insert_rules('A', "x");
  gram.insert_rules('B', "x");

  CheckEarley(gram, {"x"}, {"", "xx", "a"});
}

TEST(CFL_EarleyParsingTest, HomeWorkOneExample) {
  CFL gram({'a', 'b'}, {'S', 'T', 'Q', 'A', 'B', 'C', 'D'}, 'S');
  gram.insert_rules('S', "T|Q");
  gram.insert_rules('T', "CD");
  gram.insert_rules('Q', "AB");
  gram.insert_rules('A', "aA|aa");
  gram.insert_rules('B', "aBb|0");
  gram.insert_rules('C', "aCb|0");
  gram.insert_rules('D', "bD|0");

  CheckEarley(gram, {"ab", "aaaaaaaaabbb", "", "abb"},
              {"bababababba", "aab", "bbaaaaa"});
}

TEST(CFL_EarleyParsingTest, UselessSymbols) {
  CFL gram({'a'}, {'S'}, 'S');
  gram.insert_rules('S', "S|a");

  CheckEarley(gram, {"a"}, {""});
}

TEST(CFL_EarleyParsingTest, InherentlyAmbiguous_AnBnCmDm) {
  CFL gram({'n', '+', '*'}, {'E'}, 'E');
  gram.insert_rules('E', "E+E|E*E|n");

  CheckEarley(
      gram,
      {"n+n*n", "n+n+n*n*n", "n+n*n+n*n*n+n*n", "n", "n*n*n*n*n", "n+n+n+n+n"},
      {"", "n*n*n*", "n+nn*n+n", "n*n++n"});
}

TEST(CFL_EarleyParsingTest, HomeWorkTwoExampleStress) {
  CFL gram({'a', 'b'}, {'S'}, 'S');
  gram.insert_rules('S', "0|SS|aSb|bSa|aSaSb|aSbSa|bSaSa");

  vector<string> should_pass;
  vector<string> should_fail;

  split_strings_by_criterion(10, should_pass, should_fail);

  CheckEarley(gram, should_pass, should_fail);
}

TEST(CFL_EarleyParsingTest, Palindromes_NotLRk) {
  CFL gram({'a', 'b'}, {'S'}, 'S');
  gram.insert_rules('S', "aSa|bSb|0|a|b");

  CheckEarley(gram, {"", "a", "b", "aa", "bb", "aba", "bab", "abba", "baab"},
              {"ab", "ba", "aab", "bba"});
}

TEST(CFL_EarleyParsingTest, LALR_RR_Conflict) {
  CFL gram({'a', 'b', 'c', 'd', 'e'}, {'S', 'A', 'B'}, 'S');
  gram.insert_rules('S', "aAd|aBe|bAe|bBd");
  gram.insert_rules('A', "c");
  gram.insert_rules('B', "c");

  CheckEarley(gram, {"acd", "ace", "bce", "bcd"},
              {"ac", "bc", "a", "b", "c", "ae"});
}

TEST(CFL_EarleyEdgeCase, UnusedNonTerminal) {
  CFL gram({'a', 'b'}, {'S', 'A'}, 'S');
  gram.insert_rules('S', "a");
  gram.insert_rules('A', "b");

  CheckEarley(gram, {"a"}, {"b", "aa"});
}

TEST(CFL_EarleyEdgeCase, StartSymbolIsEpsilon) {
  CFL gram({}, {'S'}, 'S');
  gram.insert_rules('S', "0");
  CheckEarley(gram, {""}, {"a"});
}

TEST(CFL_EarleyEdgeCase, NoRuleForStartSymbol) {
  CFL gram({'a'}, {'S'}, 'S');
  CheckEarley(gram, {}, {"", "a"});
}

TEST(CFL_EarleyStressTest, DeepRecursion) {
  CFL gram({'a', 'b'}, {'S'}, 'S');
  gram.insert_rules('S', "aSb|0");

  int N = 500;
  std::string input;
  input.reserve(2 * N);
  for (int i = 0; i < N; ++i) input += 'a';
  for (int i = 0; i < N; ++i) input += 'b';

  EXPECT_TRUE(gram.earley_check(input))
      << "Failed to parse deeply recursive string";

  std::string bad_input = input + "b";
  EXPECT_FALSE(gram.earley_check(bad_input));
}

TEST(CFL_EarleyStressTest, EpsilonStorm) {
  CFL gram({'a', 'b', 'c'}, {'S', 'A', 'B', 'C', 'D', 'E', 'F'}, 'S');
  gram.insert_rules('S', "AEBBFEFBEFBEFBaBBFBFBFBBAAEAEEAEAEEAbCcDDCEBF");

  gram.insert_rules('A', "E");
  gram.insert_rules('E', "0");

  gram.insert_rules('B', "F");
  gram.insert_rules('F', "0");

  gram.insert_rules('C', "0");
  gram.insert_rules('D', "0");

  CheckEarley(gram, {"abc"}, {"", "ab", "bc", "aabc", "abcc"});
}

TEST(CFL_EarleyStressTest, NonLinearRecursionGrowthBrackets) {
  CFL gram({'{', '}'}, {'S'}, 'S');
  gram.insert_rules('S', "{S}S|0");

  CheckEarley(
      gram,
      {"", "{}", "{{}}{}", "{{{}}{}{}}",
       std::string(200, '{') + std::string(200, '}')},
      {"{", "}{", "{{}", "}}", std::string(100, '{') + std::string(101, '}')});
}

TEST(CFL_EarleyCoverageTest, TerminalNonTerminalConflict) {
  ASSERT_THROW(CFL({'a', 'b'}, {'S', 'A', 'a'}, 'S'), CFL::GrammarError);
}