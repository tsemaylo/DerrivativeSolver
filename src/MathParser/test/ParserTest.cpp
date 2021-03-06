/* Licensed to Oleg Tsemaylo under the MIT license.
 * Refer to the LICENSE.txt file in the project root for more information.
 */

/**
 * @file ParserTest.cpp
 * 
 * Tests for parser class.
 *
 * @since 24.04.2017
 * @author agor
 */

#include <gtest/gtest.h>
#include <string>
#include <list>
#include <vector>

#include "ParserImpl.h"
#include "Expression.h"
#include "ExpressionFactory.h"
#include "Constant.h"
#include "Variable.h"
#include "Sum.h"
#include "Sub.h"
#include "Mult.h"
#include "Div.h"
#include "Pow.h"
#include "StringGenerator.h"
#include "Token.h"
#include "ParserStack.h"
#include "Rule.h"

class ParserTest : public ParserImpl {
};

class FX_Parser : public testing::Test {
protected:
    virtual void SetUp() {
    }

    virtual void TearDown() {
    }
};

TEST_F(FX_Parser, parse_SimpleSummation_FunctionWithTwoArgs) {
    ParserTest parser;
    const string strExpr = "a+b";

    PExpression expr = parser.parse(strExpr);
    ASSERT_TRUE(isTypeOf<Sum>(expr));
}

TEST_F(FX_Parser, parse_SummationWithParentness_SumWithTwoArgs) {
    ParserTest parser;
    const string strExpr = "(a+b)+c";

    PExpression expr = parser.parse(strExpr);
    ASSERT_TRUE(isTypeOf<Sum>(expr));

    PSum sum = SPointerCast<Sum>(expr);
    PVariable varC = SPointerCast<Variable>(sum->rArg);
    EXPECT_EQ("c", varC->name);
    PSum sumL = SPointerCast<Sum>(sum->lArg);
    PVariable varA = SPointerCast<Variable>(sumL->lArg);
    EXPECT_EQ("a", varA->name);
    PVariable varB = SPointerCast<Variable>(sumL->rArg);
    EXPECT_EQ("b", varB->name);
}

TEST_F(FX_Parser, parse_AdditionOperations_SumWithTwoArgs) {
    ParserTest parser;
    const string strExpr = "a-b+c";

    PExpression expr = parser.parse(strExpr);
    ASSERT_TRUE(isTypeOf<Sum>(expr));

    PSum sum = SPointerCast<Sum>(expr);
    PVariable varC = SPointerCast<Variable>(sum->rArg);
    EXPECT_EQ("c", varC->name);
    PSub subL = SPointerCast<Sub>(sum->lArg);
    PVariable varA = SPointerCast<Variable>(subL->lArg);
    EXPECT_EQ("a", varA->name);
    PVariable varB = SPointerCast<Variable>(subL->rArg);
    EXPECT_EQ("b", varB->name);
}

TEST_F(FX_Parser, parse_AdditionOperationsWithParentness_SumWithTwoArgs) {
    ParserTest parser;
    const string strExpr = "-(a-b)+c";

    PExpression expr = parser.parse(strExpr);
    ASSERT_TRUE(isTypeOf<Sum>(expr));

    PSum sum = SPointerCast<Sum>(expr);
    PVariable varC = SPointerCast<Variable>(sum->rArg);
    EXPECT_EQ("c", varC->name);

    PMult multL = SPointerCast<Mult>(sum->lArg);
    PConstant constN1 = SPointerCast<Constant>(multL->lArg);
    EXPECT_DOUBLE_EQ(-1.0, constN1->value);
    PSub subL = SPointerCast<Sub>(multL->rArg);
    PVariable varA = SPointerCast<Variable>(subL->lArg);
    EXPECT_EQ("a", varA->name);
    PVariable varB = SPointerCast<Variable>(subL->rArg);
    EXPECT_EQ("b", varB->name);
}

TEST_F(FX_Parser, parse_MixedExpression_SubWithTwoArgs) {
    ParserTest parser;
    const string strExpr = "a-b*c";

    PExpression expr = parser.parse(strExpr);
    ASSERT_TRUE(isTypeOf<Sub>(expr));

    PSub sub = SPointerCast<Sub>(expr);
    PVariable varA = SPointerCast<Variable>(sub->lArg);
    EXPECT_EQ("a", varA->name);
    
    PMult multR = SPointerCast<Mult>(sub->rArg);
    PVariable varB = SPointerCast<Variable>(multR->lArg);
    EXPECT_EQ("b", varB->name);
    PVariable varC = SPointerCast<Variable>(multR->rArg);
    EXPECT_EQ("c", varC->name);
}

TEST_F(FX_Parser, parse_MixedExpressionWithParentness_SumWithTwoArgs) {
    ParserTest parser;
    const string strExpr = "-(a*b)^n+c/2";

    PExpression expr = parser.parse(strExpr);
    
    StringGenerator stringGenerator;
    expr->traverse(stringGenerator);
    
    // expecting Sum: lArg= -(a*b)^n     rArg=c/2
    ASSERT_TRUE(isTypeOf<Sum>(expr)) << stringGenerator.getLastVisitResult();
    PSum sum = SPointerCast<Sum>(expr);
    
    // expecting Div: lArg=c    rArg=2
    ASSERT_NE(nullptr, sum->rArg);
    ASSERT_TRUE(isTypeOf<Div>(sum->rArg)) << stringGenerator.getLastVisitResult();
    PDiv divC2 = SPointerCast<Div>(sum->rArg);
    
    PVariable varC = SPointerCast<Variable>(divC2->lArg);
    EXPECT_EQ("c", varC->name);
    PConstant const2 = SPointerCast<Constant>(divC2->rArg);
    EXPECT_DOUBLE_EQ(2.0, const2->value);

    // expecting Mult: lArg=-1    rArg=(a*b)^n
    ASSERT_NE(nullptr, sum->lArg);
    ASSERT_TRUE(isTypeOf<Mult>(sum->lArg)) << stringGenerator.getLastVisitResult();
    PMult multL = SPointerCast<Mult>(sum->lArg);
    
    PConstant constN1 = SPointerCast<Constant>(multL->lArg);
    EXPECT_DOUBLE_EQ(-1.0, constN1->value);
    
    // expecting Pow: lArg=a*b      rArg=n
    ASSERT_NE(nullptr, multL->rArg);
    ASSERT_TRUE(isTypeOf<Pow>(multL->rArg)) << stringGenerator.getLastVisitResult();
    PPow powN = SPointerCast<Pow>(multL->rArg);
    
    // expecting Mult. lArg=a     rArg=b
    ASSERT_NE(nullptr, powN->lArg);
    ASSERT_TRUE(isTypeOf<Mult>(powN->lArg)) << stringGenerator.getLastVisitResult();
    PMult multAB = SPointerCast<Mult>(powN->lArg);
    PVariable varA = SPointerCast<Variable>(multAB->lArg);
    EXPECT_EQ("a", varA->name);
    PVariable varB = SPointerCast<Variable>(multAB->rArg);
    EXPECT_EQ("b", varB->name);
    
    // expecting Variable
    PVariable varN = SPointerCast<Variable>(powN->rArg);
    EXPECT_EQ("n", varN->name);
}

TEST_F(FX_Parser, parse_ExpressionWithFunctions_Function) {
    ParserTest parser;
    const string strExpr = "sin(x+2)";

    PExpression expr = parser.parse(strExpr);
    ASSERT_TRUE(isTypeOf<Sin>(expr));

    PSin sin = SPointerCast<Sin>(expr);
    PSum sum = SPointerCast<Sum>(sin->arg);
    
    PVariable varX = SPointerCast<Variable>(sum->lArg);
    EXPECT_EQ("x", varX->name);
    PConstant constTwo = SPointerCast<Constant>(sum->rArg);
    EXPECT_DOUBLE_EQ(2.0, constTwo->value);
}

TEST_F(FX_Parser, parse_ExpressionWithFunctions_ExpressionTree) {
    ParserTest parser;
    const string strExpr = "cos(x^2)*sin(x+2)-ln(tan(x/2))+exp(ctan(-x)+x)";

    PExpression expr = parser.parse(strExpr);
    
    StringGenerator stringGenerator;
    expr->traverse(stringGenerator);
    
    ASSERT_TRUE(isTypeOf<Sum>(expr)) << stringGenerator.getLastVisitResult();
    PSum sum = SPointerCast<Sum>(expr);
    
    // cos(x^2)*sin(x+2)-ln(tan(x/2))
    ASSERT_NE(nullptr, sum->lArg) << stringGenerator.getLastVisitResult();
    ASSERT_TRUE(isTypeOf<Sub>(sum->lArg)) << stringGenerator.getLastVisitResult();
    PSub subL = SPointerCast<Sub>(sum->lArg);
    
    // cos(x^2)*sin(x+2)
    ASSERT_NE(nullptr, subL->lArg) << stringGenerator.getLastVisitResult();
    ASSERT_TRUE(isTypeOf<Mult>(subL->lArg)) << stringGenerator.getLastVisitResult();
    PMult multSinCos = SPointerCast<Mult>(subL->lArg);
    
    // cos(x^2)
    ASSERT_NE(nullptr, multSinCos->lArg) << stringGenerator.getLastVisitResult();
    ASSERT_TRUE(isTypeOf<Cos>(multSinCos->lArg)) << stringGenerator.getLastVisitResult();
    PCos cos = SPointerCast<Cos>(multSinCos->lArg);
    
    // x^2
    ASSERT_NE(nullptr, cos->arg) << stringGenerator.getLastVisitResult();
    ASSERT_TRUE(isTypeOf<Pow>(cos->arg)) << stringGenerator.getLastVisitResult();
    PPow powX2 = SPointerCast<Pow>(cos->arg);
    ASSERT_NE(nullptr, powX2->lArg) << stringGenerator.getLastVisitResult();
    ASSERT_TRUE(isTypeOf<Variable>(powX2->lArg)) << stringGenerator.getLastVisitResult();
    PVariable varX_1 = SPointerCast<Variable>(powX2->lArg);
    ASSERT_TRUE(isTypeOf<Constant>(powX2->rArg)) << stringGenerator.getLastVisitResult();
    PConstant const2_1 = SPointerCast<Constant>(powX2->rArg);
    ASSERT_STREQ("x", varX_1->name.c_str());
    ASSERT_DOUBLE_EQ(2.0, const2_1->value);
    
    // sin(x+2)
    ASSERT_NE(nullptr, multSinCos->rArg) << stringGenerator.getLastVisitResult();
    ASSERT_TRUE(isTypeOf<Sin>(multSinCos->rArg)) << stringGenerator.getLastVisitResult();
    PSin sin = SPointerCast<Sin>(multSinCos->rArg);
    
    // x+2
    ASSERT_NE(nullptr, sin->arg) << stringGenerator.getLastVisitResult();
    ASSERT_TRUE(isTypeOf<Sum>(sin->arg)) << stringGenerator.getLastVisitResult();
    PSum sumX2 = SPointerCast<Sum>(sin->arg);
    ASSERT_NE(nullptr, sumX2->lArg) << stringGenerator.getLastVisitResult();
    ASSERT_TRUE(isTypeOf<Variable>(sumX2->lArg)) << stringGenerator.getLastVisitResult();
    PVariable varX_2 = SPointerCast<Variable>(sumX2->lArg);
    ASSERT_TRUE(isTypeOf<Constant>(sumX2->rArg)) << stringGenerator.getLastVisitResult();
    PConstant const2_2 = SPointerCast<Constant>(sumX2->rArg);
    ASSERT_STREQ("x", varX_2->name.c_str());
    ASSERT_DOUBLE_EQ(2.0, const2_2->value);
    
    // ln(tan(x/2))
    ASSERT_NE(nullptr, subL->rArg) << stringGenerator.getLastVisitResult();
    ASSERT_TRUE(isTypeOf<Ln>(subL->rArg)) << stringGenerator.getLastVisitResult();
    PLn lnTan = SPointerCast<Ln>(subL->rArg);
    
    // tan(x/2)
    ASSERT_NE(nullptr, lnTan->arg) << stringGenerator.getLastVisitResult();
    ASSERT_TRUE(isTypeOf<Tan>(lnTan->arg)) << stringGenerator.getLastVisitResult();
    PTan tan = SPointerCast<Tan>(lnTan->arg);
    
    // x/2
    ASSERT_NE(nullptr, tan->arg) << stringGenerator.getLastVisitResult();
    ASSERT_TRUE(isTypeOf<Div>(tan->arg)) << stringGenerator.getLastVisitResult();
    PDiv divX2 = SPointerCast<Div>(tan->arg);
    ASSERT_NE(nullptr, divX2->lArg) << stringGenerator.getLastVisitResult();
    ASSERT_TRUE(isTypeOf<Variable>(divX2->lArg)) << stringGenerator.getLastVisitResult();
    PVariable varX_3 = SPointerCast<Variable>(divX2->lArg);
    ASSERT_TRUE(isTypeOf<Constant>(divX2->rArg)) << stringGenerator.getLastVisitResult();
    PConstant const2_3 = SPointerCast<Constant>(divX2->rArg);
    ASSERT_STREQ("x", varX_3->name.c_str());
    ASSERT_DOUBLE_EQ(2.0, const2_3->value);
    
    // exp(ctan(-x)+x)
    ASSERT_NE(nullptr, sum->rArg) << stringGenerator.getLastVisitResult();
    ASSERT_TRUE(isTypeOf<Exp>(sum->rArg)) << stringGenerator.getLastVisitResult();
    PExp expR = SPointerCast<Exp>(sum->rArg);
    
    // ctan(-x)+x
    ASSERT_NE(nullptr, expR->arg) << stringGenerator.getLastVisitResult();
    ASSERT_TRUE(isTypeOf<Sum>(expR->arg)) << stringGenerator.getLastVisitResult();
    PSum sumR = SPointerCast<Sum>(expR->arg);
    
    // ctan(-x)
    ASSERT_NE(nullptr, sumR->lArg) << stringGenerator.getLastVisitResult();
    ASSERT_TRUE(isTypeOf<Ctan>(sumR->lArg)) << stringGenerator.getLastVisitResult();
    PCtan ctan = SPointerCast<Ctan>(sumR->lArg);
    
    // -x
    ASSERT_NE(nullptr, ctan->arg) << stringGenerator.getLastVisitResult();
    ASSERT_TRUE(isTypeOf<Mult>(ctan->arg)) << stringGenerator.getLastVisitResult();
    PMult multNeg = SPointerCast<Mult>(ctan->arg);
    ASSERT_NE(nullptr, multNeg->lArg) << stringGenerator.getLastVisitResult();
    ASSERT_TRUE(isTypeOf<Constant>(multNeg->lArg)) << stringGenerator.getLastVisitResult();
    PConstant constNeg = SPointerCast<Constant>(multNeg->lArg);
    ASSERT_DOUBLE_EQ(-1.0, constNeg->value);
    ASSERT_NE(nullptr, multNeg->rArg) << stringGenerator.getLastVisitResult();
    ASSERT_TRUE(isTypeOf<Variable>(multNeg->rArg)) << stringGenerator.getLastVisitResult();
    PVariable varX_4 = SPointerCast<Variable>(multNeg->rArg);
    ASSERT_STREQ("x", varX_4->name.c_str());
    
    // x
    ASSERT_NE(nullptr, sumR->rArg) << stringGenerator.getLastVisitResult();
    ASSERT_TRUE(isTypeOf<Variable>(sumR->rArg)) << stringGenerator.getLastVisitResult();
    PVariable varX_5 = SPointerCast<Variable>(sumR->rArg);
    ASSERT_STREQ("x", varX_5->name.c_str());
}

TEST_F(FX_Parser, parse_ExpressionWithFunctions2_ExpressionTree) {
    ParserTest parser;
    const string strExpr = "-cos(x)^(n+3)";

    PExpression expr = parser.parse(strExpr);
    
    StringGenerator stringGenerator;
    expr->traverse(stringGenerator);
    
    ASSERT_TRUE(isTypeOf<Mult>(expr)) << stringGenerator.getLastVisitResult();
    PMult mult = SPointerCast<Mult>(expr);
    
    ASSERT_NE(nullptr, mult->lArg) << stringGenerator.getLastVisitResult();
    ASSERT_TRUE(isTypeOf<Constant>(mult->lArg)) << stringGenerator.getLastVisitResult();
    PConstant constNeg = SPointerCast<Constant>(mult->lArg);
    ASSERT_DOUBLE_EQ(-1.0, constNeg->value);
    
    // cos(x)^(n+3)
    ASSERT_NE(nullptr, mult->rArg) << stringGenerator.getLastVisitResult();
    ASSERT_TRUE(isTypeOf<Pow>(mult->rArg)) << stringGenerator.getLastVisitResult();
    PPow pow = SPointerCast<Pow>(mult->rArg);
    
    // cos(x)
    ASSERT_NE(nullptr, pow->lArg) << stringGenerator.getLastVisitResult();
    ASSERT_TRUE(isTypeOf<Cos>(pow->lArg)) << stringGenerator.getLastVisitResult();
    PCos cos = SPointerCast<Cos>(pow->lArg);
    ASSERT_NE(nullptr, cos->arg) << stringGenerator.getLastVisitResult();
    ASSERT_TRUE(isTypeOf<Variable>(cos->arg)) << stringGenerator.getLastVisitResult();
    PVariable varX = SPointerCast<Variable>(cos->arg);
    ASSERT_STREQ("x", varX->name.c_str());
    
    // (n+3)
    ASSERT_NE(nullptr, pow->rArg) << stringGenerator.getLastVisitResult();
    ASSERT_TRUE(isTypeOf<Sum>(pow->rArg)) << stringGenerator.getLastVisitResult();
    PSum sum = SPointerCast<Sum>(pow->rArg);
    ASSERT_NE(nullptr, sum->lArg) << stringGenerator.getLastVisitResult();
    ASSERT_TRUE(isTypeOf<Variable>(sum->lArg)) << stringGenerator.getLastVisitResult();
    PVariable varN = SPointerCast<Variable>(sum->lArg);
    ASSERT_TRUE(isTypeOf<Constant>(sum->rArg)) << stringGenerator.getLastVisitResult();
    PConstant const2 = SPointerCast<Constant>(sum->rArg);
    ASSERT_STREQ("n", varN->name.c_str());
    ASSERT_DOUBLE_EQ(3.0, const2->value);
}

TEST_F(FX_Parser, parse_CanParseFloatingPointNumbers_Success) {
    ParserTest parser;
    const string strExpr = "3.7 + 5,3";

    PExpression expr = parser.parse(strExpr);
    
    ASSERT_TRUE(isTypeOf<Sum>(expr));
    
    PSum typedExpr=SPointerCast<Sum>(expr);
    ASSERT_TRUE(isTypeOf<Constant>(typedExpr->lArg));
    ASSERT_TRUE(isTypeOf<Constant>(typedExpr->rArg));
    
    PConstant termL=SPointerCast<Constant>(typedExpr->lArg);
    PConstant termR=SPointerCast<Constant>(typedExpr->rArg);
    EXPECT_DOUBLE_EQ(3.7, termL->value);
    EXPECT_DOUBLE_EQ(5.3, termR->value);
}

TEST_F(FX_Parser, parse_CanParseUpperCase_Success) {
    ParserTest parser;
    const string strExpr = "X+Y";

    PExpression expr = parser.parse(strExpr);
    
    ASSERT_TRUE(isTypeOf<Sum>(expr));
    
    PSum typedExpr=SPointerCast<Sum>(expr);
    ASSERT_TRUE(isTypeOf<Variable>(typedExpr->lArg));
    ASSERT_TRUE(isTypeOf<Variable>(typedExpr->rArg));
    
    PVariable termL=SPointerCast<Variable>(typedExpr->lArg);
    PVariable termR=SPointerCast<Variable>(typedExpr->rArg);
    EXPECT_EQ("X", termL->name);
    EXPECT_EQ("Y", termR->name);
}

TEST_F(FX_Parser, parse_ImplicitMultiplication_Success) {
    vector<string> tests;
    vector<PExpression> expectedResults;
    
    
    tests.push_back("(x+1)(x-1)");
    expectedResults.push_back(createMult(
        createSum(createVariable("x"), createConstant(1)),
        createSub(createVariable("x"), createConstant(1))
    ));
    
    tests.push_back("4-2x");
    expectedResults.push_back(createSub(
        createConstant(4),
        createMult(createConstant(2), createVariable("x"))
    ));
    
    tests.push_back("2x - 4");
    expectedResults.push_back(createSub(
        createMult(createConstant(2), createVariable("x")),
        createConstant(4)
    ));
    
    tests.push_back("2cos(x)");
    expectedResults.push_back(createMult(
        createConstant(2),
        createCos(createVariable("x"))
    ));
    
    tests.push_back("a+7(x+1)");
    expectedResults.push_back(createSum(
        createVariable("a"),
        createMult(
            createConstant(7),
            createSum(createVariable("x"), createConstant(1))
        )
    ));
    
    tests.push_back("2x^2");
    expectedResults.push_back(createMult(
        createConstant(2),
        createPow(createVariable("x"), createConstant(2))
    ));

    
    for(unsigned int testId=0; testId < tests.size(); testId++){
        ParserTest parser;
        PExpression actResult;
        EXPECT_NO_THROW(actResult=parser.parse(tests[testId])) << "Test ID=" << testId << " threw an exception!";
        string expected=to_string(expectedResults[testId]);
        string actual=to_string(actResult);
        EXPECT_TRUE(equals(expectedResults[testId], actResult)) << 
                "Result does not match for test ID=" << testId << "! " 
                << expected << " != " << actual;
    }
}

TEST_F(FX_Parser, parse_NotANumber_ParsingException) {
    ParserTest parser;
    const string strExpr = "...";

    ASSERT_THROW(parser.parse(strExpr), ParsingException);
}

TEST_F(FX_Parser, parse_FailedParentness_ParsingException) {
    ParserTest parser;
    ASSERT_THROW(parser.parse("a+(b+(c+(d+e)+k)"), ParsingException);
}

TEST_F(FX_Parser, parse_FailedParentnessUnexpectedClosingBracket_ParsingException) {
    ParserTest parser;
    ASSERT_THROW(parser.parse("a+)b)"), ParsingException);
}

TEST_F(FX_Parser, parse_FailedParentnessNoClosingBracketAtTheExndOfTheStaring_ParsingException) {
    ParserTest parser;
    ASSERT_THROW(parser.parse("("), ParsingException);
}

TEST_F(FX_Parser, parse_UnknownCharacter_ParsingException) {
    ParserTest parser;
    ASSERT_THROW(parser.parse("a+'b'"), ParsingException);
}

TEST_F(FX_Parser, parse_WhiteSpaceString_ParsingException) {
    ParserTest parser;
    ASSERT_THROW(parser.parse("     "), ParsingException);
}
