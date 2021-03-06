/* Licensed to Oleg Tsemaylo under the MIT license.
 * Refer to the LICENSE.txt file in the project root for more information.
 */

/**
 * @file    SumConstantsRule.h
 * 
 * Definition of the SumConstantsRule class.
 * 
 * @author   agor
 * @since 11.09.2017
 */

#ifndef SUMCONSTANTSRULE_H
#define SUMCONSTANTSRULE_H

#include "OptimizationRule.tpp"

#include <Sum.h>
#include <TraverseException.h>

/**
 * The optimization rule to optimize the sum of two constants.
 */
class SumConstantsRule : public OptimizationRule<PSum>{
public:
    SumConstantsRule(PSum _expression);

    bool apply() throw(TraverseException) final;
};

#endif /* SUMCONSTANTSRULE_H */

