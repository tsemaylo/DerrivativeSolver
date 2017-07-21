/**
 * @file   Mult.h
 * 
 * Abstraction for expression for multiplication operation.
 * 
 * @since 04.07.2017
 * @author agor
 */

#ifndef MULT_H
#define	MULT_H

#include <memory>
#include "Expression.h"

class Mult : public Expression
{
public:
	shared_ptr<Expression> lArg;
	shared_ptr<Expression> rArg;

	Mult();
	
	void traverse(Visitor & ) const throw(TraverseException) final;
};

#endif	/* MULT_H */

