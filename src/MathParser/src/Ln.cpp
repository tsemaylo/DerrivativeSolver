/* Licensed to Oleg Tsemaylo under the MIT license.
 * Refer to the LICENSE.txt file in the project root for more information.
 */

/**
 * @file    Ln.cpp
 * 
 * Implemntation of Ln class.
 * 
 * @author  agor
 * @since   22.08.2017
 */

#include "Ln.h"
#include "Visitor.h"
#include "TraverseException.h"

Ln::Ln() : Expression(ELn) {
}

bool Ln::isComplete() const {
    return (this->arg!=nullptr);
}

void Ln::traverse(Visitor& visitor) const throw(TraverseException) {
    visitor.visit(shared_from_this());
}


