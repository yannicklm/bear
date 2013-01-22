/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief Implementation of the bear::expr::boolean_constant class.
 * \author Julien Jorge.
 */
#include "expr/boolean_constant.hpp"

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 * \param v The constant from which we take the value.
 */
bear::expr::boolean_constant::boolean_constant( bool v )
  : m_value(v)
{

} // boolean_constant::boolean_constant()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create a copy of this expression.
 */
bear::expr::base_boolean_expression*
bear::expr::boolean_constant::clone() const
{
  return new boolean_constant(*this);
} // boolean_constant::boolean_constant()

/*----------------------------------------------------------------------------*/
/**
 * \brief Evaluate the expression.
 */
bool bear::expr::boolean_constant::evaluate() const
{
  return m_value;
} // boolean_constant::evaluate()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the value of the constant.
 * \param b The new value.
 */
void bear::expr::boolean_constant::set_value( bool b )
{
  m_value = b;
} // boolean_constant::set_value()