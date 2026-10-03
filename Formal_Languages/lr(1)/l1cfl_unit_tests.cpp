#include <sstream>
#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "lr.h"

void CheckParsing(const L1CFL& grammar,
                  const std::vector<std::string>& should_pass,
                  const std::vector<std::string>& should_fail) {
  for (const auto& s : should_pass) {
    L1CFL::Verdict v = grammar.parse(s);
    EXPECT_TRUE(v.status) << "Should ACCEPT: '" << s
                          << "' but REJECTED. Info: " << v.info;
  }

  for (const auto& s : should_fail) {
    L1CFL::Verdict v = grammar.parse(s);
    EXPECT_FALSE(v.status) << "Should REJECT: '" << s << "' but ACCEPTED.";
  }
}

bool SetupAndBuild(L1CFL& grammar) {
  std::cout << "--- Начинается построение таблицы ---" << std::endl;
  bool success = grammar.build_parsing_table(std::cout);

  if (!success) {
    std::cerr << "!!! ОШИБКА: Обнаружен конфликт в грамматике. См. выше. !!!"
              << std::endl;
  }

  return success;
}

TEST(L1CFL_ExceptionTest, InvalidSymbolsConstructor) {
  EXPECT_THROW(L1CFL({'@'}, {'S'}, 'S'), L1CFL::GrammarError);
  EXPECT_THROW(L1CFL({'$'}, {'S'}, 'S'), L1CFL::GrammarError);

  EXPECT_THROW(L1CFL({'a'}, {'S', 'a'}, 'S'), L1CFL::GrammarError);

  EXPECT_THROW(L1CFL({'a'}, {'A'}, 'S'), L1CFL::GrammarError);

  EXPECT_THROW(L1CFL({'A'}, {'S'}, 'S'), L1CFL::GrammarError);
  EXPECT_THROW(L1CFL({'a'}, {'s'}, 's'), L1CFL::GrammarError);
}

TEST(L1CFL_ExceptionTest, InvalidRuleInsertion) {
  L1CFL gram({'a'}, {'S', 'A'}, 'S');

  EXPECT_THROW(gram.insert_rule('X', "a"), L1CFL::GrammarError);

  EXPECT_THROW(gram.insert_rule('S', "b"), L1CFL::GrammarError);

  EXPECT_THROW(gram.insert_rule('S', "A B"), L1CFL::GrammarError);
}

TEST(L1CFL_ParsingTest, DyckLanguage) {
  L1CFL gram({'(', ')'}, {'S'}, 'S');
  gram.insert_rules('S', "0|(S)S");
  gram.insert_rules('S', "");
  ASSERT_TRUE(SetupAndBuild(gram));
  gram.build_parsing_table(std::cout);
  CheckParsing(gram, {"", "()", "(())", "()()", "()(())"},
               {"(", ")", ")(", "(()", "(())("});
}

TEST(L1CFL_ParsingTest, LeftRecursion) {
  L1CFL gram({'a', 'b'}, {'S'}, 'S');
  gram.insert_rules('S', "Sa|b");

  ASSERT_TRUE(SetupAndBuild(gram));
  CheckParsing(gram, {"b", "ba", "baa", "baaaaa"}, {"", "a", "ab", "bba"});
}

TEST(L1CFL_ParsingTest, ArithmeticExpressions) {
  L1CFL gram({'i', '+', '*', '(', ')'}, {'E', 'T', 'F'}, 'E');
  gram.insert_rules('E', "E+T|T");
  gram.insert_rules('T', "T*F|F");
  gram.insert_rules('F', "(E)|i");

  ASSERT_TRUE(SetupAndBuild(gram));
  CheckParsing(gram, {"i", "i+i", "i*i", "i+i*i", "(i+i)*i", "((i))"},
               {"", "+", "i+", "+i", "()", "i()i"});
}

TEST(L1CFL_AdvancedTest, LR1_Not_SLR1_LValue) {
  L1CFL gram({'=', '*', 'i'}, {'S', 'L', 'R'}, 'S');
  gram.insert_rules('S', "L=R|R");
  gram.insert_rules('L', "*R|i");
  gram.insert_rules('R', "L");

  ASSERT_TRUE(SetupAndBuild(gram));
  CheckParsing(gram, {"i", "*i", "**i", "i=i", "*i=i"},
               {"=", "i=", "=i", "**"});
}

TEST(L1CFL_AdvancedTest, LALR_RR_Conflict) {
  L1CFL gram({'a', 'b', 'c', 'd', 'e'}, {'S', 'A', 'B'}, 'S');
  gram.insert_rules('S', "aAd|aBe|bAe|bBd");
  gram.insert_rules('A', "c");
  gram.insert_rules('B', "c");

  ASSERT_TRUE(SetupAndBuild(gram));
  CheckParsing(gram, {"acd", "ace", "bce", "bcd"}, {"ac", "bc", "a", "b", "c"});
}

TEST(L1CFL_AdvancedTest, TrickyLR1) {
  L1CFL gram({'a', 'b', 'c', 'd'}, {'S', 'A'}, 'S');
  gram.insert_rules('S', "Aa|bAc|dc|bda");
  gram.insert_rules('A', "d");

  ASSERT_TRUE(SetupAndBuild(gram));
  CheckParsing(gram, {"da", "bdc", "dc", "bda"}, {"dda", "bdca"});
}

TEST(L1CFL_AdvancedTestConflict, CyclicGrammarHandlingConflict) {
  L1CFL gram({'a'}, {'S'}, 'S');
  gram.insert_rules('S', "S|a");

  ASSERT_FALSE(SetupAndBuild(gram));
}

TEST(L1CFL_ConflictTest, ShiftReduce_Ambiguity) {
  L1CFL gram({'a', 'b'}, {'S'}, 'S');
  gram.insert_rules('S', "SaS|b");
  ASSERT_FALSE(SetupAndBuild(gram));
}

TEST(L1CFL_ConflictTest, ReduceReduce_Simple) {
  L1CFL gram({'x'}, {'S', 'A', 'B'}, 'S');
  gram.insert_rules('S', "A|B");
  gram.insert_rules('A', "x");
  gram.insert_rules('B', "x");
  ASSERT_FALSE(SetupAndBuild(gram));
}

TEST(L1CFL_ConflictTest, DanglingElse) {
  L1CFL gram({'i', 'e', 'a'}, {'S'}, 'S');
  gram.insert_rules('S', "iSeS|iS|a");
  ASSERT_FALSE(SetupAndBuild(gram));
}

TEST(L1CFL_ConflictTest, Palindromes_NotLRk) {
  L1CFL gram({'a', 'b'}, {'S'}, 'S');
  gram.insert_rules('S', "aSa|bSb|0");
  ASSERT_FALSE(SetupAndBuild(gram));
}

TEST(L1CFL_EdgeCase, UnusedNonTerminal) {
  L1CFL gram({'a', 'b'}, {'S', 'A'}, 'S');
  gram.insert_rules('S', "a");
  gram.insert_rules('A', "b");

  ASSERT_TRUE(SetupAndBuild(gram));
  CheckParsing(gram, {"a"}, {"b", "aa"});
}

TEST(L1CFL_EdgeCase, StartSymbolIsEpsilon) {
  L1CFL gram({}, {'S'}, 'S');
  gram.insert_rules('S', "0");
  ASSERT_TRUE(SetupAndBuild(gram));
  CheckParsing(gram, {""}, {"a"});
}

TEST(L1CFL_EdgeCase, NoRuleForStartSymbol) {
  L1CFL gram({'a'}, {'S'}, 'S');
  ASSERT_TRUE(SetupAndBuild(gram));
  CheckParsing(gram, {}, {"", "a"});
}

TEST(L1CFL_StressTest, DeepRecursionStack) {
  L1CFL gram({'a', 'b'}, {'S'}, 'S');
  gram.insert_rules('S', "aSb|0");
  ASSERT_TRUE(SetupAndBuild(gram));

  int N = 5000;
  std::string input;
  input.reserve(2 * N);
  for (int i = 0; i < N; ++i) input += 'a';
  for (int i = 0; i < N; ++i) input += 'b';

  L1CFL::Verdict v = gram.parse(input);
  EXPECT_TRUE(v.status) << "Failed to parse deeply recursive string (N=5000)";

  std::string bad_input = input + "b";
  EXPECT_FALSE(gram.parse(bad_input).status);
}

TEST(L1CFL_StressTest, EpsilonStorm) {
  L1CFL gram({'a', 'b', 'c'}, {'S', 'A', 'B', 'C', 'D', 'E', 'F'}, 'S');
  gram.insert_rules('S', "AEBBFEFBEFBEFBaBBFBFBFBBAAEAEEAEAEEAbCcDDCEBF");

  gram.insert_rules('A', "E");
  gram.insert_rules('E', "0");

  gram.insert_rules('B', "F");
  gram.insert_rules('F', "0");

  gram.insert_rules('C', "0");
  gram.insert_rules('D', "0");

  ASSERT_TRUE(SetupAndBuild(gram));

  CheckParsing(gram, {"abc"}, {"", "ab", "bc", "aabc"});
}

TEST(L1CFL_StressTest, NonLinearRecursionGrowth) {
  L1CFL gram({'a', 'b'}, {'S'}, 'S');
  gram.insert_rules('S', "aSbS|0");
  ASSERT_TRUE(SetupAndBuild(gram));

  CheckParsing(gram,
               {"", "aabb", "aaabbb", "aaaabbbb",
                std::string(10000, 'a') + std::string(10000, 'b')},
               {"abb", "ba", "aaa", "bbb",
                std::string(10000, 'a') + std::string(10000, 'b') + "b"});
}

TEST(L1CFL_StressTest, LongSequenceParsing) {
  L1CFL gram({'a'}, {'S'}, 'S');

  std::string rule_rhs;
  int N = 1000;
  for (int i = 0; i < N; ++i) rule_rhs += 'a';

  gram.insert_rules('S', rule_rhs);
  ASSERT_TRUE(SetupAndBuild(gram));

  CheckParsing(gram, {rule_rhs}, {rule_rhs + "a", rule_rhs.substr(0, N - 1)});
}

TEST(L1CFL_StressTest, NonLinearRecursionGrowthBrackets) {
  L1CFL gram({'{', '}'}, {'S'}, 'S');
  gram.insert_rules('S', "{S}S|0");

  ASSERT_TRUE(SetupAndBuild(gram));

  CheckParsing(gram,
               {"", "{}", "{{}}{}", "{{{}}{}{}}",
                std::string(10000, '{') + std::string(10000, '}')},
               {"{", "}{", "{{}", "}}",
                std::string(10000, '{') + std::string(10001, '}')});
}

TEST(L1CFL_CoverageTest, TerminalNonTerminalConflict) {
  ASSERT_THROW(L1CFL({'a', 'b'}, {'S', 'A', 'a'}, 'S'), L1CFL::GrammarError)
      << "Должна быть ошибка GrammarError, когда символ 'a' является и "
         "терминалом, и нетерминалом.";
}

TEST(L1CFL_CoverageTest, ParseBeforeTableBuilt) {
  L1CFL gram({'a'}, {'S'}, 'S');
  gram.insert_rules('S', "a");

  L1CFL::Verdict v = gram.parse("a");

  ASSERT_FALSE(v.status)
      << "Парсинг должен завершиться неудачей, если таблица не готова.";
  ASSERT_EQ(v.info, "Table is not ready");
}

TEST(L1CFL_ExceptionTest, TerminalNonTerminalConflictMessage) {
  const char conflicting_symbol = '#';

  try {
    L1CFL({'#', 'b'}, {'S', 'A', '#'}, 'S');
    FAIL() << "Ожидалось исключение L1CFL::GrammarError, но его не было.";

  } catch (const L1CFL::GrammarError& e) {
    std::string expected_message_part =
        "' can't be both terminal and non terminal.";
    std::string expected_symbol_part =
        "Symbol '" + std::string(1, conflicting_symbol);

    std::string actual_message = e.what();

    ASSERT_NE(actual_message.find(expected_symbol_part), std::string::npos)
        << "Сообщение об ошибке не содержит ожидаемый символ: "
        << actual_message;

    ASSERT_NE(actual_message.find(expected_message_part), std::string::npos)
        << "Сообщение об ошибке не содержит ожидаемую фразу: "
        << actual_message;

    SUCCEED();

  } catch (...) {
    FAIL() << "Было выброшено неожиданное исключение, отличное от "
              "L1CFL::GrammarError.";
  }
}