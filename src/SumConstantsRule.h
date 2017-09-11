/**
 * @file    SumConstantsRule.cpp
 * 
 * Definition of the SumConstantsRule class.
 * 
 * @author   agor
 * @since 11.09.2017
 */

#ifndef SUMCONSTANTSRULE_H
#define SUMCONSTANTSRULE_H

#include "OptimizationRule.h"

#include <Sum.h>
#include <Expression.h>
#include <TraverseException.h>

/**
 * The optimization rule to optimize the sum of two constants.
 */
class SumConstantsRule : public OptimizationRule{
public:
    SumConstantsRule(PSum _expression);

    bool apply() throw(TraverseException) final;
};

#endif /* SUMCONSTANTSRULE_H */
