/* Licensed to Oleg Tsemaylo under the MIT license.
 * Refer to the LICENSE.txt file in the project root for more information.
 */

/**
 * @file ParserImpl.cpp
 *
 * @brief Implementation of the Parser.
 *
 * @since 25.03.2016
 * @Author: agor
 */
#include "ExceptionThrower.h"
#include "Parser.h"
#include "ParserImpl.h"

#include "ParserStack.h"
#include "Token.h"
#include "Constant.h"
#include "Variable.h"
#include "Sum.h"
#include "Sub.h"
#include "Div.h"
#include "Mult.h"
#include "Pow.h"
#include "Sin.h"
#include "Cos.h"
#include "Tan.h"
#include "Ctan.h"
#include "Ln.h"
#include "Exp.h"
#include "RulePowLV.h"
#include "RulePowRV.h"
#include "RuleMultLV.h"
#include "RuleMultRV.h"
#include "RuleDivLV.h"
#include "RuleDivRV.h"
#include "RuleSumLV.h"
#include "RuleSumRV.h"
#include "RuleSubLV.h"
#include "RuleSubRV.h"
#include "RuleFunction.h"
#include "RuleNoSignMult.h"
#include "ExpressionFactory.h"

#include "ParserStack.h"

ParserImpl::ParserImpl() {
    unsigned int n = 0;
    // initialize grammar
    // Rule #17
    this->grammar[n++] = make_unique<RuleFunction<Sin>>();
    // Rule #18
    this->grammar[n++] = make_unique<RuleFunction<Cos>>();
    // Rule #19
    this->grammar[n++] = make_unique<RuleFunction<Tan>>();
    // Rule #20
    this->grammar[n++] = make_unique<RuleFunction<Ctan>>();
    // Rule #21
    this->grammar[n++] = make_unique<RuleFunction<Ln>>();
    // Rule #22
    this->grammar[n++] = make_unique<RuleFunction<Exp>>();
    // Rule #23
    this->grammar[n++] = make_unique<RulePowLV>();
    // Rule #24
    this->grammar[n++] = make_unique<RulePowRV>();
    // Rule #25
    this->grammar[n++] = make_unique<RuleMultLV>();
    // Rule #26
    this->grammar[n++] = make_unique<RuleMultRV>();
    // Rule #35
    this->grammar[n++] = make_unique<RuleNoSignMult>();
    // Rule #27
    this->grammar[n++] = make_unique<RuleDivLV>();
    // Rule #28
    this->grammar[n++] = make_unique<RuleDivRV>();
    // Rule #29
    this->grammar[n++] = make_unique<RuleSumLV>();
    // Rule #30 #31
    this->grammar[n++] = make_unique<RuleSumRV>();
    // Rule #32
    this->grammar[n++] = make_unique<RuleSubLV>();
    // Rule #33 #34
    this->grammar[n++] = make_unique<RuleSubRV>();
}

bool ParserImpl::isAlpha(char c) const {
    // assuming ASCII

    if (c >= 'a' && c <= 'z') {
        return true;
    }

    if (c >= 'A' && c <= 'Z') {
        return true;
    }

    return false;
}

bool ParserImpl::isNumeric(char c) const {
    // assuming ASCII

    if ((c >= '0' && c <= '9') || c == '.' || c == ',') {
        return true;
    }

    return false;
}

bool ParserImpl::isGroupBracket(char c) const {
    return (c == '(' || c == ')');
}

bool ParserImpl::isArithOperation(char c) const {
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '\\' || c == '^');
}

bool ParserImpl::isWhitespace(char c) const {
    return (c == ' ' || c == '\t');
}

list<Token> ParserImpl::getTokens(const string &strExpr) const {
    list<Token> tokens;

    /**
     * traverse the string and split it into valid tokens.
     * The token is
     * - sequence alphabet characters and numbers
     * - groups brackets "(" ")"
     * - math operations: +, -, *, /, ^
     * - sequence of numbers
     * This characters are ignored
     * - whitespace characters
     * Not allowed characters:
     * - all other characters
     */
    string tokenValue;
    TokenType tokenType = TNoToken;
    for (uint i = 0; i < strExpr.length(); i++) {
        char c = strExpr[i];
        TokenType tokenTypeOfSymbol = TNoToken;
        if (this->isNumeric(c)) {
            tokenTypeOfSymbol = TNumeric;
        } else if (this->isAlpha(c)) {
            tokenTypeOfSymbol = TAlphaNumeric;
        } else if (this->isGroupBracket(c)) {
            tokenTypeOfSymbol = TGroupBracket;
        } else if (this->isArithOperation(c)) {
            tokenTypeOfSymbol = TOperation;
        } else if (this->isWhitespace(c)) {
            // ignore
            continue;
        } else {
            THROW(ParsingException, "Unknown character.", "'" + to_string(c) + "'");
        }

        if (tokenType != tokenTypeOfSymbol || tokenType == TGroupBracket) {
            // new token

            // save old token
            if (!tokenValue.empty()) {
                Token token(tokenValue, tokenType);
                tokens.push_back(token);
            }
            // prepare a new token
            tokenValue.clear();
            tokenType = tokenTypeOfSymbol;
        }
        //  add symbol to token
        tokenValue += c;
    }

    // put the last token into list
    if (!tokenValue.empty()) {
        Token token(tokenValue, tokenType);
        tokens.push_back(token);
    }

    return tokens;
}

bool ParserImpl::doReduce(ParserStack &stack, const Token &lookAheadToken) const {
    // go through the list of rules and check if it is applicable to the provided stack
    for (auto &rule : this->grammar) {
        // try to apply the rule to reduce the stack
        if (rule->apply(stack, lookAheadToken)) {
            return true;
        }
    }

    return false;
}

PExpression ParserImpl::createOperation(const string opSymbol) const throw (ParsingException) {
    if (opSymbol == "+") {
        return createSum();
    }
    if (opSymbol == "-") {
        return createSub();
    }
    if (opSymbol == "*") {
        return createMult();
    }
    if (opSymbol == "/" || opSymbol == "\\") {
        return createDiv();
    }
    // onlyone possible option since getToken() considers nothing else
    return createPow();
}

PExpression ParserImpl::createFunction(const string opSymbol) const throw (ParsingException) {
    if (opSymbol == "sin") {
        return createSin();
    }
    if (opSymbol == "cos") {
        return createCos();
    }
    if (opSymbol == "tan") {
        return createTan();
    }
    if (opSymbol == "ctan") {
        return createCtan();
    }
    if (opSymbol == "ln") {
        return createLn();
    }
    // onlyone possible option since Token::isFunction() considers nothing else
    return createExp();
}

list<Token>::const_iterator ParserImpl::findEndOfParentheses(list<Token>::const_iterator start, list<Token>::const_iterator end) const throw (ParsingException) {
    int openingBrackets = 1; // we already have so far one oppening bracket
    int closingBrackets = 0; // ...and it is not yet closed
    string trace = "";

    while (start != end) {
        trace += start->value;

        if (start->type != TGroupBracket) {
            ++start;
            continue;
        }

        if (start->value == "(") {
            openingBrackets++;
        }

        if (start->value == ")") {
            closingBrackets++;
        }

        if (openingBrackets == closingBrackets) {
            // fount it
            return start;
        }

        ++start;
    }

    // ops, brackets are not closed
    THROW(ParsingException, "No closing bracket has been found.", trace);
}

list<Token>::const_iterator ParserImpl::shiftToStack(list<Token>::const_iterator current, list<Token>::const_iterator end, ParserStack &stack) const throw (ParsingException) {
    Token token = *current;

    PExpression stackExpression;
    if (TNumeric == token.type) {
        try {
            stackExpression = createConstant(token.value);
        } catch (std::exception &ex) {
            THROW(ParsingException, string("Not a number token. (") + ex.what() + ")", token.value);
        }
    } else if (TOperation == token.type) {
        stackExpression = createOperation(token.value);
    } else if (TAlphaNumeric == token.type) {
        // check if the token represents function
        if (token.isFunction()) {
            stackExpression = createFunction(token.value);
        } else {
            // assuming it is a variable
            stackExpression = createVariable(token.value);
        }
    } else if (TGroupBracket == token.type) {
        // it must be an opening bracket
        if (current->value == ")") {
            THROW(ParsingException, "Unexpected closing bracket ')'.", "N.A.");
        }
        ParserStack subStack;
        ++current; // this one is bracket - take the next one
        if (current == end) {
            THROW(ParsingException, "Unexpect end of the expression.", "No closing bracket at the end of the string.");
        }
        list<Token>::const_iterator endParentheses = findEndOfParentheses(current, end);
        this->doParseTokens(current, endParentheses, subStack);
        stackExpression = subStack.front();
        current = endParentheses;
    }
        
    if(stackExpression != nullptr) {
        stack.push_back(stackExpression);

         // go to the next iten in the token list
         ++current;
    }
    
    return current;
}

Token ParserImpl::getLookAheadToken(list<Token>::const_iterator current, list<Token>::const_iterator end) const {
    if (current != end) {
        return (*current);
    }

    return Token("eof", TNoToken);
}

void ParserImpl::doParseTokens(list<Token>::const_iterator start, list<Token>::const_iterator end, ParserStack &stack) const throw (ParsingException) {
    // opened question: can this function to be called recursively

    // LR parsing => shift-reduce method (bottom-up)
    // the method is chosen since it considers only forward scanning of tokens 

    while (start != end) {
        start = shiftToStack(start, end, stack);

        Token lookAheadToken = getLookAheadToken(start, end);

        // reduce the stack untill no other posibility to reduce is available
        while (this->doReduce(stack, lookAheadToken)) {
            // ???
        }
    }

    // at the end we should have only one element in the stack that means
    // everything is reduced and parsed
    if (stack.size() != 1) {
        // if the stack is not completely reduced 
        // then probably grammar is not complete
        // or the syntax of the provided expression is incorrect

        THROW(ParsingException, "The specified expression is ambiguous. Not able to completely reduce syntax tree.", to_string(stack));
    }
}

PExpression ParserImpl::parseTokens(const list<Token> &tokens) const {
    ParserStack stack;
    this->doParseTokens(tokens.begin(), tokens.end(), stack);
    return stack.front();
}

const PExpression ParserImpl::parse(const string &strExpr) const throw (ParsingException) {
    return this->parseTokens(this->getTokens(strExpr));
}

PExpression parse(const std::string &strExpr) throw (ParsingException) {
    ParserImpl parserImpl;
    return parserImpl.parse(strExpr);
}