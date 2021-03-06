/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief A class to manage images resources.
 * \author Julien Jorge
 */
#ifndef __VISUAL_IMAGE_MANAGER_HPP__
#define __VISUAL_IMAGE_MANAGER_HPP__

#include "visual/image.hpp"
#include "visual/shader_program.hpp"

#include <iostream>
#include <map>
#include <string>

#include "visual/class_export.hpp"

namespace bear
{
  namespace visual
  {
    /**
     * \brief A class to manage images resources.
     * \author Julien Jorge
     */
    class VISUAL_EXPORT image_manager
    {
    public:
      void clear();
      void load_image( std::string name, std::istream& file );

      void clear_images();
      void restore_image( std::string name, std::istream& file );

      image get_image( std::string name ) const;
      void get_image_names( std::vector<std::string>& names ) const;

      bool exists( std::string name ) const;

      void load_shader_program( std::string name, std::istream& file );

      void clear_shader_programs();
      void restore_shader_program( std::string name, std::istream& file );

      shader_program get_shader_program( std::string name ) const;

      void get_shader_program_names( std::vector<std::string>& names ) const;
      bool has_shader_program( std::string name ) const;
      
    private:
      /** \brief All the images. */
      std::map<std::string, image> m_images;

      /** \brief All the shader programs. */
      std::map<std::string, shader_program> m_shader_program;

    }; // class image_manager

  } // namespace visual
} // namespace bear

#endif // __VISUAL_IMAGE_MANAGER_HPP__
