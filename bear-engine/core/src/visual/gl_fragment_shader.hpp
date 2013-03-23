/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief A class to use OpenGL fragment shaders.
 * \author Julien Jorge
 */
#ifndef __VISUAL_GL_FRAGMENT_SHADER_HPP__
#define __VISUAL_GL_FRAGMENT_SHADER_HPP__

#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h>

#include <iostream>
#include "visual/class_export.hpp"

namespace bear
{
  namespace visual
  {
    /**
     * \brief A fragment shader finalizes the rendering of the pixels.
     * \author Julien Jorge
     */
    class VISUAL_EXPORT gl_fragment_shader
    {
    public:
      gl_fragment_shader( std::istream& p );
      ~gl_fragment_shader();
    
      GLuint shader_id() const;

    private:
      void log_errors() const;
    
    private:
      /** \brief The OpenGL identifier of the fragment shader. */
      GLuint m_fragment_shader;
    
    }; // gl_fragment_shader

  } // namespace visual
} // namespace bear

#endif // __VISUAL_GL_FRAGMENT_SHADER_HPP__