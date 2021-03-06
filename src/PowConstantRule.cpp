/* Licensed to Oleg Tsemaylo under the MIT license.
 * Refer to the LICENSE.txt file in the project root for more information.
 */

/**
 * @file     PowConstantRule.cpp
 * 
 * Implementation of the PowConstantRule class.
 * 
 * @author   agor
 * @since 16.11.2017
 */

#include "PowConstantRule.h"
#include <cmath>
#include <ExpressionFactory.h>
#include <Constant.h>
#include "Doubles.h"

PowConstantRule::PowConstantRule(PPow _expression) : OptimizationRule(_expression){
}

bool PowConstantRule::apply() throw(TraverseException){    
    if(!isTypeOf<Constant>(this->expression->rArg)){
        return false;
    }
    
    PConstant exponent=SPointerCast<Constant>(this->expression->rArg);
    if(equal(exponent->value, 0.0)){
        this->optimizedExpression=createConstant(1.0);
        return true;
    }
    
    if(equal(exponent->value, 1.0)){
        this->optimizedExpression=this->expression->lArg;
        return true;
    }
    
    if(isTypeOf<Constant>(this->expression->lArg)){
        PConstant base=SPointerCast<Constant>(this->expression->lArg);
        this->optimizedExpression=createConstant(std::pow(base->value, exponent->value));
        return true;
    }
    
    return false;
}


