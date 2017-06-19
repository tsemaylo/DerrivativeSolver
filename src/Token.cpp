/**
 * @file   Token.cpp
 * 
 * @brief  POD for simple token
 * 
 * @since 30.05.2017
 * @author agor
 */

#include "Token.h"

Token::Token(string _value, TokenType _type) : value(_value), type(_type) {}

string Token::getValue() const{
	return this->value;
}

TokenType Token::getType() const{
	return this->type;
}
