/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the bear::input::mouse class.
 * \author Julien Jorge
 */
#include "input/mouse.hpp"

#include "bear_gettext.hpp"

#include <SDL/SDL.h>
#include <algorithm>
#include <claw/assert.hpp>

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
bear::input::mouse::mouse()
{
  update_position();
} // mouse::mouse()

/*----------------------------------------------------------------------------*/
/**
 * \brief Convert a mouse_code to a human-readable string.
 * \param k The code to convert.
 */
std::string bear::input::mouse::get_name_of( mouse_code b )
{
  switch (b)
    {
    case mc_left_button: return "left click"; break;
    case mc_middle_button: return "middle click"; break;
    case mc_right_button: return "right click"; break;
    case mc_wheel_up: return "wheel up"; break;
    case mc_wheel_down: return "wheel down"; break;
    default:
      {
        CLAW_FAIL( "Invalid mouse code given to mouse::get_name_of()" );
        return "invalid mouse code";
      }
    }
} // mouse::get_name_of()

/*----------------------------------------------------------------------------*/
/**
 * \brief Convert a mouse_code to a human-readable string translated with
 *        gettext.
 * \param k The code to convert.
 */
std::string bear::input::mouse::get_translated_name_of( mouse_code b )
{
  return bear_gettext( get_name_of(b).c_str() );
} // mouse::get_translated_name_of()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get a mouse_code from its human-readable name.
 * \param n The name of the code.
 */
bear::input::mouse::mouse_code
bear::input::mouse::get_button_named( const std::string& n )
{
  for (mouse_code i=mc_range_min; i<=mc_range_max; ++i)
    if ( get_name_of(i) == n )
      return i;

  return mc_invalid;
} // mouse::get_button_named()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get an iterator on the first pressed button.
 */
bear::input::mouse::const_iterator bear::input::mouse::begin() const
{
  return m_pressed_buttons.begin();
} // mouse::begin()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get an iterator after the last pressed button.
 */
bear::input::mouse::const_iterator bear::input::mouse::end() const
{
  return m_pressed_buttons.end();
} // mouse::end()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tell if no keys are pressed.
 */
bool bear::input::mouse::empty() const
{
  return m_pressed_buttons.empty();
} // mouse::empty()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the position of the mouse.
 */
const claw::math::coordinate_2d<unsigned int>&
bear::input::mouse::get_position() const
{
  return m_position;
} // mouse::get_position()

/*----------------------------------------------------------------------------*/
/**
 * \brief Re-read the status of all keys.
 * \pre The caller is an instance of bear::input::system.
 */
void bear::input::mouse::refresh()
{
  update_position();
  m_pressed_buttons.clear();

  SDL_Event e;

  while ( SDL_PeepEvents(&e, 1, SDL_GETEVENT, SDL_MOUSEEVENTMASK) == 1 )
    {
      SDL_MouseButtonEvent* evt = reinterpret_cast<SDL_MouseButtonEvent*>(&e);

      if ( (evt->type == SDL_MOUSEBUTTONDOWN) && (evt->state == SDL_PRESSED) )
        m_pressed_buttons.push_back( sdl_button_to_local( evt->button ) );
    }
} // mouse::refresh()

/*----------------------------------------------------------------------------*/
/**
 * \brief Update the position of the mouse.
 */
void bear::input::mouse::update_position()
{
  int x, y;

  SDL_GetMouseState( &x, &y );
  SDL_Surface* s( SDL_GetVideoSurface() );

  if ( s!=NULL )
    y = s->h - y;

  m_position.set(x, y);
} // mouse::update_position()

/*----------------------------------------------------------------------------*/
/**
 * \brief Convert a SDLK_* value to the corresponding mouse_code.
 * \param sdl_val The SDL value to convert.
 * \param shift Tell if a shift button is considered pressed.
 * \param alt Tell if an alt button is considered pressed.
 */
bear::input::mouse::mouse_code bear::input::mouse::sdl_button_to_local
( unsigned int sdl_val ) const
{
  switch(sdl_val)
    {
    case SDL_BUTTON_LEFT : return mc_left_button; break;
    case SDL_BUTTON_MIDDLE : return mc_middle_button; break;
    case SDL_BUTTON_RIGHT : return mc_right_button; break;
    case SDL_BUTTON_WHEELUP : return mc_wheel_up; break;
    case SDL_BUTTON_WHEELDOWN : return mc_wheel_down; break;
    default: return mc_invalid;
    }

} // mouse::sdl_mouse_to_local()
