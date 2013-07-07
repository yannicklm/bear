/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the bear::engine::freedesktop_game_filesystem class.
 * \author Julien Jorge
 */
#include "engine/system/freedesktop_game_filesystem.hpp"

#include <claw/logger.hpp>
#include <claw/system_info.hpp>
#include <boost/filesystem/convenience.hpp>

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 * \param game_name The name of the game
 */
bear::engine::freedesktop_game_filesystem::freedesktop_game_filesystem
( std::string game_name )
  : m_game_name( game_name )
{

} // freedesktop_game_filesystem::freedesktop_game_filesystem()

/*----------------------------------------------------------------------------*/
/**
 * \brief Creates a dynamically allocated copy of this instance.
 */
bear::engine::freedesktop_game_filesystem*
bear::engine::freedesktop_game_filesystem::clone() const
{
  return new freedesktop_game_filesystem( *this );
} // freedesktop_game_filesystem::clone()

/*----------------------------------------------------------------------------*/
/**
 * \brief Returns the full path of a configuration file of the game.
 * \param name The name of the configuration file.
 */
std::string
bear::engine::freedesktop_game_filesystem::get_custom_config_file_name
( std::string name )
{
  return get_custom_game_file
    ( name, get_freedesktop_directory( "XDG_CONFIG_HOME", ".config" ) );
} // freedesktop_game_filesystem::get_custom_config_file_name()

/*----------------------------------------------------------------------------*/
/**
 * \brief Returns the full path of a file generated by the user in the game.
 * \param name The name of the file.
 */
std::string
bear::engine::freedesktop_game_filesystem::get_custom_data_file_name
( std::string name )
{
  return get_custom_game_file
    ( name, get_freedesktop_directory( "XDG_DATA_HOME", ".local/share" ) );
} // freedesktop_game_filesystem::get_custom_data_file_name()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the path of a file in the user's game directory.
 * \param name The name of the file.
 * \param dir The directory where the game's directory, containing the file,
 *        will be created.
 */
std::string bear::engine::freedesktop_game_filesystem::get_custom_game_file
( std::string name, std::string dir ) const
{
  std::string result;
  const std::string game_directory( get_game_directory(dir) );

  if ( !game_directory.empty() )
    {
      boost::filesystem::path path( game_directory );
      path /= name;
      result = path.string();
    }
  else
    result = name;

  return result;
} // freedesktop_game_filesystem::get_custom_game_file()

/*----------------------------------------------------------------------------*/
/**
 * \brief Gets, and creates, the personal game directory of the user.
 * \param dir The directory where the game's directory will be created.
 */
std::string
bear::engine::freedesktop_game_filesystem::get_game_directory
( std::string dir ) const
{
  boost::filesystem::path path( dir );

  std::string result;
  std::string subdir = get_name_as_filename( m_game_name );

  path /= boost::filesystem::path(subdir);

  try
    {
      boost::filesystem::create_directories(path);
      result = path.string();
    }
  catch( std::exception& e )
    {
      claw::logger << claw::log_error << "Can't create game directory '"
                   << path.string() << "': " << e.what() << std::endl;
    }

  return result;
} // freedesktop_game_filesystem::get_game_directory()

/*----------------------------------------------------------------------------*/
/**
 * \brief Gets, and creates, a directory according to the freedesktop.org
 *        specifications.
 * \param env The environment variable declaring the directory.
 * \param default_dir The default directory to use if env is not set, relatively
 *        to the user's home directory.
 */
std::string
bear::engine::freedesktop_game_filesystem::get_freedesktop_directory
( std::string env, std::string default_dir ) const
{
  boost::filesystem::path dir;
  std::string env_dir( claw::system_info::get_environment(env) );

  if ( env_dir.empty() )
    dir =
      boost::filesystem::path
      ( claw::system_info::get_user_directory() )
      / default_dir;
  else
    dir = env_dir;

  return dir.string();
} // freedesktop_game_filesystem::get_freedesktop_directory()
