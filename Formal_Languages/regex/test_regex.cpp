#include "gtest/gtest.h"
#include "regex.h" 
#include <stdexcept>
#include <unordered_set>

namespace regex_pattern_detail {
    extern int k_mod;
    extern int l_target;
}

using regex_pattern_detail::Residues;
using regex_pattern_detail::regex_exception;

class RegexResidueCalcTest : public ::testing::Test {
protected:
    void SetUp() override {
        regex_pattern_detail::k_mod = 5;
    }
};

TEST_F(RegexResidueCalcTest, CalcUnion_Basic) {
    Residues r1 = {1, 3};
    Residues r2 = {2, 4};
    Residues expected = {1, 2, 3, 4};
    ASSERT_EQ(regex_pattern_detail::calc_union(r1, r2), expected);
}

TEST_F(RegexResidueCalcTest, CalcUnion_WithOverlap) {
    Residues r1 = {1, 3, 4};
    Residues r2 = {3, 4, 0};
    Residues expected = {0, 1, 3, 4};
    ASSERT_EQ(regex_pattern_detail::calc_union(r1, r2), expected);
}

TEST_F(RegexResidueCalcTest, CalcConcat_Basic) {
    Residues r1 = {1, 2};
    Residues r2 = {3, 4};
    Residues expected = {0, 1, 4};
    ASSERT_EQ(regex_pattern_detail::calc_concat(r1, r2), expected);
}

TEST_F(RegexResidueCalcTest, CalcConcat_EmptyOperands) {
    Residues r1 = {};
    Residues r2 = {1, 2};
    Residues expected_empty = {};
    ASSERT_EQ(regex_pattern_detail::calc_concat(r1, r2), expected_empty);
    ASSERT_EQ(regex_pattern_detail::calc_concat(r2, r1), expected_empty);
}

TEST_F(RegexResidueCalcTest, CalcKleene_Simple) {
    Residues r = {2};
    Residues expected = {0, 1, 2, 3, 4};
    ASSERT_EQ(regex_pattern_detail::calc_kleene(r), expected);
}

TEST_F(RegexResidueCalcTest, CalcKleene_ContainsZero) {
    Residues r = {0, 3};
    Residues expected = {0, 1, 2, 3, 4};
    ASSERT_EQ(regex_pattern_detail::calc_kleene(r), expected);
}

TEST_F(RegexResidueCalcTest, CalcKleene_Empty) {
    Residues r = {};
    Residues expected = {0};
    ASSERT_EQ(regex_pattern_detail::calc_kleene(r), expected);
}

class RpnResiduesTest : public ::testing::Test {
protected:
    void SetUp() override {
        regex_pattern_detail::k_mod = 3;
    }
};

TEST_F(RpnResiduesTest, Rpn_SingleChar) {
    Residues expected = {1};
    ASSERT_EQ(regex_pattern_detail::calc_regex_residues("a"), expected);
}

TEST_F(RpnResiduesTest, Rpn_Epsilon) {
    Residues expected = {0};
    ASSERT_EQ(regex_pattern_detail::calc_regex_residues("1"), expected);
}

TEST_F(RpnResiduesTest, Rpn_Concatenation) {
    Residues expected = {2};
    ASSERT_EQ(regex_pattern_detail::calc_regex_residues("ab."), expected);
}

TEST_F(RpnResiduesTest, Rpn_Union) {
    Residues expected = {0, 1};
    ASSERT_EQ(regex_pattern_detail::calc_regex_residues("a1+"), expected);
}

TEST_F(RpnResiduesTest, Rpn_Kleene) {
    Residues expected = {0, 1, 2};
    ASSERT_EQ(regex_pattern_detail::calc_regex_residues("a*"), expected);
}

TEST_F(RpnResiduesTest, Rpn_Complex) {
    Residues expected = {0, 1, 2};
    ASSERT_EQ(regex_pattern_detail::calc_regex_residues("ab.c+*"), expected);
}

TEST_F(RpnResiduesTest, Rpn_Error_MissingOperand_Concat) {
    ASSERT_THROW(regex_pattern_detail::calc_regex_residues("a."), regex_exception);
}

TEST_F(RpnResiduesTest, Rpn_Error_MissingOperand_Union) {
    ASSERT_THROW(regex_pattern_detail::calc_regex_residues("a+"), regex_exception);
}

TEST_F(RpnResiduesTest, Rpn_Error_MissingOperand_Kleene) {
    ASSERT_THROW(regex_pattern_detail::calc_regex_residues("*"), regex_exception);
}

TEST_F(RpnResiduesTest, Rpn_Error_UnidentifiedCharacter) {
    ASSERT_THROW(regex_pattern_detail::calc_regex_residues("a!"), regex_exception);
}

TEST_F(RpnResiduesTest, Rpn_Error_AlphabetChar) {
    ASSERT_THROW(regex_pattern_detail::calc_regex_residues("d"), regex_exception);
}

TEST_F(RpnResiduesTest, Rpn_Error_WrongNumberOfOperands) {
    ASSERT_THROW(regex_pattern_detail::calc_regex_residues("ab"), regex_exception);
}


class RegexModPatternTest : public ::testing::Test {};

TEST_F(RegexModPatternTest, Main_HappyPath_Match) {
    ASSERT_EQ(regex_mod_pattern("a", 3, 1), "YES");
}

TEST_F(RegexModPatternTest, Main_HappyPath_NoMatch) {
    ASSERT_EQ(regex_mod_pattern("a", 3, 2), "NO");
}

TEST_F(RegexModPatternTest, Main_Complex_Match) {
    ASSERT_EQ(regex_mod_pattern("a*", 5, 0), "YES");
}

TEST_F(RegexModPatternTest, Main_EmptyRegex) {
    ASSERT_EQ(regex_mod_pattern("", 5, 1), "NO");
}

TEST_F(RegexModPatternTest, Main_KIsOne) {
    ASSERT_EQ(regex_mod_pattern("a*", 1, 0), "YES");
}

TEST_F(RegexModPatternTest, Main_KIsOne_LIsZero) {
    ASSERT_EQ(regex_mod_pattern("a", 1, 0), "YES");
}

TEST_F(RegexModPatternTest, Main_KIsOne_WithUnion) {
    ASSERT_EQ(regex_mod_pattern("a1+", 1, 0), "YES");
}
TEST_F(RegexModPatternTest, Main_Concat_Basic) {
    ASSERT_EQ(regex_mod_pattern("ab.", 5, 2), "YES");
}

TEST_F(RegexModPatternTest, Main_Concat_NoMatch) {
    ASSERT_EQ(regex_mod_pattern("aaa..", 5, 4), "NO");
}

TEST_F(RegexModPatternTest, Main_Global_Yes) {
    ASSERT_EQ(regex_mod_pattern("ab+c.aba.*.bac.+.+*", 3, 2), "YES");
}

TEST_F(RegexModPatternTest, Main_Global_No) {
    ASSERT_EQ(regex_mod_pattern("acb..bab.c.*.ab.ba.+.+*a.", 3, 0), "NO");
}

TEST_F(RegexModPatternTest, Main_Concat_WithEpsilon) {
    ASSERT_EQ(regex_mod_pattern("a1.", 3, 1), "YES");
}


TEST_F(RegexModPatternTest, Main_Union_WithEpsilon) {
    ASSERT_EQ(regex_mod_pattern("a1+", 2, 0), "YES");
}
TEST_F(RegexModPatternTest, Main_Kleene_CoversAll) {
    ASSERT_EQ(regex_mod_pattern("a*", 4, 3), "YES");
}

TEST_F(RegexModPatternTest, Main_Kleene_Partial_NoMatch) {
    ASSERT_EQ(regex_mod_pattern("aa.*", 4, 1), "NO");
}

TEST_F(RegexModPatternTest, Main_Kleene_Partial_Match) {
    ASSERT_EQ(regex_mod_pattern("aa.*", 4, 2), "YES");
}

TEST_F(RegexModPatternTest, Main_Kleene_Epsilon) {
    ASSERT_EQ(regex_mod_pattern("1*", 5, 0), "YES");
}

TEST_F(RegexModPatternTest, Main_ParamError_K_Zero) {
    ASSERT_THROW(regex_mod_pattern("a", 0, 0), regex_exception);
}

TEST_F(RegexModPatternTest, Main_ParamError_K_Negative) {
    ASSERT_THROW(regex_mod_pattern("a", -5, 0), regex_exception);
}

TEST_F(RegexModPatternTest, Main_ParamError_L_Negative) {
    ASSERT_THROW(regex_mod_pattern("a", 5, -1), regex_exception);
}

TEST_F(RegexModPatternTest, Main_ParamError_L_TooBig) {
    ASSERT_THROW(regex_mod_pattern("a", 5, 5), regex_exception);
    ASSERT_THROW(regex_mod_pattern("a", 5, 6), regex_exception);
}

TEST_F(RegexModPatternTest, Main_RegexError_Propagated) {
    ASSERT_THROW(regex_mod_pattern("a+", 5, 1), regex_exception);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}