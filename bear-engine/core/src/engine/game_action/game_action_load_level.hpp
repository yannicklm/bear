/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief Tell a game to load a level.
 * \author Julien Jorge
 */
#ifndef __ENGINE_GAME_ACTION_LOAD_LEVEL_HPP__
#define __ENGINE_GAME_ACTION_LOAD_LEVEL_HPP__

#include "engine/game_action/game_action.hpp"

#include <string>

#include "engine/class_export.hpp"

namespace bear
{
  namespace engine
  {
    /**
     * \brief Tell a game to load a level.
     * \author Julien Jorge
     */
    class ENGINE_EXPORT game_action_load_level:
      public game_action
    {
    public:
      game_action_load_level( const std::string& path );

      bool apply( game_local_client& the_game );

    private:
      /** \brief The path of the level to load. */
      std::string m_path;

    }; // class game_action_load_level
  } // namespace engine
} // namespace bear

#endif // __ENGINE_GAME_ACTION_LOAD_LEVEL_HPP__