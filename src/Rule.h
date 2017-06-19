/**
 * @file   Rule.h
 * 
 * @brief Interface for syntax parsing rules.
 * 
 * @since 01.06.2017
 * @author agor
 */

#ifndef RULE_H
#define	RULE_H

#include <vector>

using namespace std;

class Expression;

/**
 * Represents the interface for any possible parsing rule.
 */
class Rule{
public:
	virtual bool apply(vector<Expression *> *nonterminals) const = 0;
};

#endif	/* RULE_H */
