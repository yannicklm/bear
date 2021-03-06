/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the bear::bool_game_variable_getter class.
 * \author Sébastien Angibaud
 */

#include "engine/function/bool_game_variable_getter.hpp"

/*----------------------------------------------------------------------------*/
/**
 * \brief Contructor.
 */
bear::engine::bool_game_variable_getter::bool_game_variable_getter()
{
  set_default_value(false);
} // bool_game_variable_getter::bool_game_variable_getter()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create a copy of this expression.
 */
bear::engine::bool_game_variable_getter*
bear::engine::bool_game_variable_getter::clone() const
{
  return new bool_game_variable_getter(*this);
} // bool_game_variable_getter::clone()

/*----------------------------------------------------------------------------*/
/**
 * \brief Evaluate the expression.
 */
bool bear::engine::bool_game_variable_getter::evaluate() const
{
  return (*this)();
} // bool_game_variable_getter::evaluate()
