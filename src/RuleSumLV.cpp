/**
 * @file   RuleSumLV.cpp
 * 
 * Grammar rule to handle summation operation (left hand operand).
 * 
 * @since 22.06.2017
 * @author agor
 */

#include "bootstrap.h"
#include "RuleSumLV.h"
#include "Sum.h"

using namespace std;

bool RuleSumLV::apply(ParserStack &stack) const throw(ParsingException) {
	// search for the patterns
	
	ParserStack::iterator item=stack.begin();
	ParserStack::iterator nextItem=stack.begin();
	++nextItem;
	
	// iterating through the "frame" of two items
	for( ;nextItem!=stack.end(); ++item, ++nextItem){		
		if((*nextItem)->type != ESum){
			continue;
		}
		// ok we found "+" opeartor
		// then the left operand comes to + operationand item is removed from the stack
			
		// if operation of the left side is incomplete then 
		// throw a parsing exception
		if(!(*item)->isComplete())
		{
			THROW(ParsingException, "Incomplete expression on the left side of '+'.", to_string(stack));
		}
				
		// the expression on the left side is correct
		// initialize l-side argument
		// see Grammar rule #29
		dynamic_pointer_cast<Sum>(*nextItem)->lArg=(*item);
		// reduce the stack
		stack.erase(item);
			
		return true;
	}

	return false;
}
