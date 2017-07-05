/**
 * @file   Sub.cpp
 * 
 * Expression for subtraction operation.
 * 
 * @since 04.07.2017
 * @author agor
 */

#include "Sub.h"
#include "Visitor.h"

Sub::Sub() : Expression(ESub) {}

void Sub::traverse(Visitor &visitor) const throw(TraverseException) {
	visitor.visit(*this);
}
