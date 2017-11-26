/* Licensed to Oleg Tsemaylo under the MIT license.
 * Refer to the LICENSE.txt file in the project root for more information.
 */

/* 
 * \file   SolverApplication.h
 * 
 * \brief Implementation of Application class.
 * 
 * \version $Revision: $ (SVN)
 * \date $Date: $ (SVN)
 * \author agor
 */

#include "SolverApplication.h"

#include <iostream>
#include <Expression.h>
#include <Parser.h>
#include <ParserFactory.h>

#include "Differentiator.h"
#include "Optimizer.h"

using namespace std;

SolverApplication::SolverApplication() {
}

SolverApplication::~SolverApplication() {
}

void SolverApplication::setStrExpression(const string strExpression) {
    this->strExpression = strExpression;
}

string SolverApplication::getStrExpression() const {
    return this->strExpression;
}

void SolverApplication::setStrVariable(const string strVariable) {
    this->strVariable = strVariable;
}

string SolverApplication::getStrVariable() const {
    return this->strVariable;
}

int SolverApplication::run() {
    int returnCode=0;
    std::unique_ptr<Parser> parser=createParser();
    try {
        PExpression parsed = parser->parse(this->strExpression);
        PExpression preOptimized=optimize(parsed);
        PExpression differentiated=differentiate(preOptimized, this->strVariable);
        PExpression optimized=optimize(differentiated);

        cout << to_string(optimized) << endl;
    } catch (ParsingException ex) {
        cout << "ERROR: " << ex.what();
        returnCode=1;
    } catch (TraverseException ex) {
        cout << "ERROR: " << ex.what();
        returnCode=1;
    }
    
    return returnCode;
}
