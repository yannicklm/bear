/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the bear::visual::gl_screen class.
 * \author Julien Jorge
 */
#include "visual/gl_screen.hpp"

#include "visual/gl_error.hpp"
#include "visual/gl_image.hpp"
#include "visual/gl_shader_program.hpp"
#include "visual/shader_program.hpp"
#include "visual/sprite.hpp"

#include <claw/exception.hpp>
#include <claw/assert.hpp>
#include <claw/logger.hpp>

#include "visual/gl.hpp"

#include <limits>
#include <list>



/*----------------------------------------------------------------------------*/
/**
 * \brief Creates a new setter for a given program.
 * \param program The identifier of the shader program in which the variables
 *        are set.
 */
bear::visual::gl_screen::uniform_setter::uniform_setter( GLuint program )
  : m_program( program )
{

} // gl_screen::uniform_setter::uniform_setter()

/*----------------------------------------------------------------------------*/
/**
 * \brief Sets the value of an integer uniform.
 * \param name The name of the uniform.
 * \param value The value to assign to the uniform.
 */
void
bear::visual::gl_screen::uniform_setter::operator()
( std::string name, int value ) const
{
  glUniform1i( glGetUniformLocation( m_program, name.c_str() ), value);
  VISUAL_GL_ERROR_THROW();
} // gl_screen::uniform_setter::operator()()

/*----------------------------------------------------------------------------*/
/**
 * \brief Sets the value of a float uniform.
 * \param name The name of the uniform.
 * \param value The value to assign to the uniform.
 */
void
bear::visual::gl_screen::uniform_setter::operator()
( std::string name, double value ) const
{
  glUniform1f( glGetUniformLocation( m_program, name.c_str() ), value);
  VISUAL_GL_ERROR_THROW();
} // gl_screen::uniform_setter::operator()()

/*----------------------------------------------------------------------------*/
/**
 * \brief Sets the value of a boolean uniform.
 * \param name The name of the uniform.
 * \param value The value to assign to the uniform.
 */
void
bear::visual::gl_screen::uniform_setter::operator()
( std::string name, bool value ) const
{
  glUniform1i( glGetUniformLocation( m_program, name.c_str() ), value);
  VISUAL_GL_ERROR_THROW();
} // gl_screen::uniform_setter::operator()()




/*----------------------------------------------------------------------------*/
/**
 * \brief Global initializations common to all gl_screens. Must be called at the
 *        begining of your program.
 */
void bear::visual::gl_screen::initialize()
{
  if ( !SDL_WasInit(SDL_INIT_VIDEO) )
    if ( SDL_InitSubSystem(SDL_INIT_VIDEO) != 0 )
      throw claw::exception( SDL_GetError() );

  if ( SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 ) != 0 )
    {
      SDL_QuitSubSystem(SDL_INIT_VIDEO);
      throw claw::exception( SDL_GetError() );
    }

  for (unsigned int i=0; i!=SDL_USEREVENT; ++i)
    SDL_EventState( i, SDL_DISABLE );

  SDL_EventState( SDL_QUIT, SDL_ENABLE );

#ifdef _WIN32
  SDL_EventState( SDL_KEYDOWN, SDL_ENABLE );
  SDL_EventState( SDL_BUTTON_WHEELUP, SDL_ENABLE );
  SDL_EventState( SDL_BUTTON_WHEELDOWN, SDL_ENABLE );
#endif
} // gl_screen::initialize()

/*----------------------------------------------------------------------------*/
/**
 * \brief Global uninitializations common to all gl_screens. Must be called at
 *        the end of your program.
 */
void bear::visual::gl_screen::release()
{
  SDL_QuitSubSystem(SDL_INIT_VIDEO);
} // gl_screen::release()

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 * \param size Size of the gl_screen.
 * \param title The title of the window created.
 * \param full Tell if the window is full gl_screen or not.
 */
bear::visual::gl_screen::gl_screen
( const claw::math::coordinate_2d<unsigned int>& size,
  const std::string& title, bool full )
  : m_size(size), m_screenshot_buffer(NULL), m_title(title)
{
  SDL_VideoInfo const * const video_info = SDL_GetVideoInfo();
  m_display_size =
    screen_size_type( video_info->current_w, video_info->current_h );
  
  fullscreen(full);
  m_need_restoration = false;

  SDL_WM_SetCaption( title.c_str(), NULL );
  glEnable(GL_TEXTURE_2D);

#ifdef BEAR_USE_GL_DEPTH_TEST
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
#endif

  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
} // gl_screen::gl_screen() [constructor]

/*----------------------------------------------------------------------------*/
/**
 * \brief Destructor.
 */
bear::visual::gl_screen::~gl_screen()
{
  delete[] m_screenshot_buffer;
} // gl_screen::~gl_screen)

/*----------------------------------------------------------------------------*/
/**
 * \brief Sets a new dimension for the resulting projection to match the size of
 *        the screen.
 */
void bear::visual::gl_screen::resize_view()
{
  glViewport( 0, 0, m_window_size.x, m_window_size.y );
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho( 0, m_size.x, 0, m_size.y, -1, 0 );
  glMatrixMode(GL_MODELVIEW);

  VISUAL_GL_ERROR_THROW();
} // gl_screen::resize_view()

/*----------------------------------------------------------------------------*/
/**
 * \brief Turn fullscreen mode on/off.
 * \param b Tell if we want a fullscreen mode.
 */
void bear::visual::gl_screen::fullscreen( bool b )
{
  set_video_mode(m_size.x, m_size.y, b);
} // gl_screen::fullscreen()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the size of the gl_screen.
 */
claw::math::coordinate_2d<unsigned int>
bear::visual::gl_screen::get_size() const
{
  return m_size;
} // gl_screen::get_size()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the size of the container of the screen.
 */
claw::math::coordinate_2d<unsigned int>
bear::visual::gl_screen::get_container_size() const
{
  const SDL_VideoInfo* info = SDL_GetVideoInfo();

  return claw::math::coordinate_2d<unsigned int>
    ( info->current_w, info->current_h ); 
} // gl_screen::get_container_size()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tell if the environment needs to be restored.
 */
bool bear::visual::gl_screen::need_restoration() const
{
  return m_need_restoration;
} // gl_screen::need_restoration()

/*----------------------------------------------------------------------------*/
/**
 * \brief Inform that the environment had been restored.
 */
void bear::visual::gl_screen::set_restored()
{
  claw::logger << claw::log_verbose << "Screen is restored." << std::endl;
  m_need_restoration = false;
} // gl_screen::set_restored()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the color of the background, used to clear the screen.
 * \param c The color.
 */
void bear::visual::gl_screen::set_background_color( const color_type& c )
{
  const GLfloat max
    ( std::numeric_limits<claw::graphic::rgb_pixel::component_type>::max() );

  glClearColor( (GLfloat)c.components.red / max,
                (GLfloat)c.components.green / max,
                (GLfloat)c.components.blue / max,
                (GLfloat)c.components.alpha / max );
} // gl_screen::set_background_color()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the color of the background, used to clear the screen.
 */
bear::visual::color_type bear::visual::gl_screen::get_background_color() const
{
  const GLfloat max
    ( std::numeric_limits<claw::graphic::rgb_pixel::component_type>::max() );

  GLfloat c[4];
  glGetFloatv( GL_COLOR_CLEAR_VALUE, c );

  color_type result;
  result.components.red = max * c[0];
  result.components.green = max * c[1];
  result.components.blue = max * c[2];
  result.components.alpha = max * c[3];

  return result;
} // gl_screen::get_background_color()

/*----------------------------------------------------------------------------*/
/**
 * \brief Initialize the rendering process.
 */
void bear::visual::gl_screen::begin_render()
{
  VISUAL_GL_ERROR_THROW();

  while ( !m_shader.empty() )
    pop_shader();

  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  m_z_position = 0;

  resize_view();
} // gl_screen::begin_render()

/*----------------------------------------------------------------------------*/
/**
 * \brief Draw a sprite on the gl_screen.
 * \param pos On gl_screen position of the sprite.
 * \param s The sprite to draw.
 */
void bear::visual::gl_screen::render
( const position_type& pos, const sprite& s )
{
  if ( s.has_transparency() )
    {
      glEnable(GL_BLEND);
      VISUAL_GL_ERROR_THROW();
    }

  glColor4f( s.get_red_intensity(), s.get_green_intensity(),
             s.get_blue_intensity(), s.get_opacity() );

  const gl_image* impl = static_cast<const gl_image*>(s.get_image().get_impl());
  glBindTexture( GL_TEXTURE_2D, impl->texture_id() );
  VISUAL_GL_ERROR_THROW();

  if ( s.get_angle() == 0 )
    {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      VISUAL_GL_ERROR_THROW();
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      VISUAL_GL_ERROR_THROW();
    }
  else
    {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      VISUAL_GL_ERROR_THROW();
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      VISUAL_GL_ERROR_THROW();
    }

  render_sprite( pos, s );

  if ( s.has_transparency() )
    {
      glDisable(GL_BLEND);
      VISUAL_GL_ERROR_THROW();
    }
} // gl_screen::render()

/*----------------------------------------------------------------------------*/
/**
 * \brief Stop the rendering process.
 * \return false if this screen has been closed by the user.
 */
bool bear::visual::gl_screen::end_render()
{
  glFlush();
  SDL_GL_SwapBuffers();
  VISUAL_GL_ERROR_THROW();

  return !is_closed();
} // gl_screen::end_render()

/*----------------------------------------------------------------------------*/
/**
 * \brief Draw a line.
 * \param color The color of the line.
 * \param p The points of the line.
 * \param w The width of the line.
 * \param close Tell if the line come back to the first coordinate.
 */
void bear::visual::gl_screen::draw_line
( const color_type& color, const std::vector<position_type>& p,
  double w, bool close )
{
  if ( w <= 0 )
    return;

  glBindTexture( GL_TEXTURE_2D, 0 );
  VISUAL_GL_ERROR_THROW();

  glLineWidth(w);

  const GLfloat max =
    std::numeric_limits<color_type::component_type>::max();

  if (color.components.alpha != max)
    {
      glEnable(GL_BLEND);
      VISUAL_GL_ERROR_THROW();
    }

  glBegin(GL_LINE_STRIP);
  {
    glColor4f( (GLfloat)color.components.red / max,
         (GLfloat)color.components.green / max,
         (GLfloat)color.components.blue / max,
         (GLfloat)color.components.alpha / max );

    for ( unsigned int i=0; i!=p.size(); ++i )
      glVertex3f( p[i].x, p[i].y, m_z_position );

    if ( close )
      glVertex3f( p[0].x, p[0].y, m_z_position );
  }
  glEnd();
  VISUAL_GL_ERROR_THROW();

  update_z_position();

  if (color.components.alpha != max)
    {
      glDisable(GL_BLEND);
      VISUAL_GL_ERROR_THROW();
    }
} // gl_screen::draw_line()

/*----------------------------------------------------------------------------*/
/**
 * \brief Draw a filled polygon.
 * \param color The color of the polygon.
 * \param p The points of the polygon.
 */
void bear::visual::gl_screen::draw_polygon
( const color_type& color, const std::vector<position_type>& p )
{
  glBindTexture( GL_TEXTURE_2D, 0 );
  VISUAL_GL_ERROR_THROW();

  const GLfloat max = std::numeric_limits<color_type::component_type>::max();

  if (color.components.alpha != max)
    {
      glEnable(GL_BLEND);
      VISUAL_GL_ERROR_THROW();
    }

  glBegin(GL_QUADS);
  {
    glColor4f( (GLfloat)color.components.red / max,
         (GLfloat)color.components.green / max,
         (GLfloat)color.components.blue / max,
         (GLfloat)color.components.alpha / max );

    for ( unsigned int i=0; i!=p.size(); ++i )
      glVertex3f( p[i].x, p[i].y, m_z_position );
  }
  glEnd();
  VISUAL_GL_ERROR_THROW();

  update_z_position();

  if (color.components.alpha != max)
    {
      glDisable(GL_BLEND);
      VISUAL_GL_ERROR_THROW();
    }
} // gl_screen::draw_polygon()

/*----------------------------------------------------------------------------*/
/**
 * \brief Sets the shader program to apply for the next render commands.
 * \param p The program to apply.
 */
void bear::visual::gl_screen::push_shader( const shader_program& p )
{
  m_shader.push_back( p );

  if ( p.is_valid() )
    use_program( p );
} // gl_screen::push_shader()

/*----------------------------------------------------------------------------*/
/**
 * \brief Removes the last shader program passed to push_shader.
 */
void bear::visual::gl_screen::pop_shader()
{
  if ( m_shader.empty() )
    {
      claw::logger << claw::log_warning << "There is no shader to pop."
                   << std::endl;
      return;
    }

  m_shader.pop_back();

  typedef std::vector<shader_program>::const_reverse_iterator iterator_type;
  bool valid_found(false);

  for ( iterator_type it = m_shader.rbegin();
        !valid_found && (it != m_shader.rend()); ++it )
    if ( it->is_valid() )
      {
        use_program( *it );
        valid_found = true;
      }

  if ( !valid_found )
    glUseProgram( 0 );
} // gl_screen::pop_shader()

/*----------------------------------------------------------------------------*/
/**
 * \brief Do a screen shot.
 * \param img The image in which we save the content of the gl_screen.
 */
void bear::visual::gl_screen::shot( claw::graphic::image& img ) const
{
  GLint p[4];
  glGetIntegerv( GL_VIEWPORT, p );
  const unsigned int w = p[2];
  const unsigned int h = p[3];

  img.set_size( w, h );
  const std::size_t pixels_count(w * h);

  glReadPixels( 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, m_screenshot_buffer );
  VISUAL_GL_ERROR_THROW();

  for ( claw::graphic::rgba_pixel_8* it=m_screenshot_buffer;
        it!=m_screenshot_buffer + pixels_count;
        ++it )
    it->components.alpha = 255;

  for (unsigned int y=0; y!=h; ++y)
    std::copy( m_screenshot_buffer + y * w,
               m_screenshot_buffer + (y+1) * w,
               img[h - y - 1].begin() );
} // gl_screen::shot()

/*----------------------------------------------------------------------------*/
/**
 * \brief Render a sprite with transformation (flip or mirror).
 * \param pos On screen position of the sprite.
 * \param s The sprite to draw.
 */
void bear::visual::gl_screen::render_sprite
( const position_type& pos, const sprite& s )
{
  if ( (s.clip_rectangle().width == 0) || (s.clip_rectangle().height == 0 ) )
    return;

  const claw::math::box_2d<GLdouble> clip_vertices = get_texture_clip(s);

  typedef claw::math::coordinate_2d<GLdouble> coord_double;

  coord_double render_coord[4];
  get_render_coord( pos, s, render_coord );

  render_image( render_coord, clip_vertices );
} // gl_screen::render_sprite()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the coordinates of the corners of a sprite after transformation
 *        (mirror, flip, rotation, etc.).
 * \param pos On screen position of the sprite.
 * \param s The sprite of which we want the corners.
 * \param result An array of 4 entries that will receive the coordinates of the
 *        corners, clockwise, starting from the top left corner.
 */
void
bear::visual::gl_screen::get_render_coord
( const position_type& pos, const sprite& s,
  claw::math::coordinate_2d<GLdouble>* result ) const
{
  typedef claw::math::coordinate_2d<GLdouble> coord_double;

  GLdouble bottom( pos.y );
  GLdouble top( bottom + s.height() );
  GLdouble left( pos.x );
  GLdouble right( left + s.width() );

  if ( s.is_mirrored() )
    std::swap(left, right);

  if ( s.is_flipped() )
    std::swap(top, bottom);

  const coord_double center = pos + s.get_size() / 2;
  const double a = s.get_angle();

  result[0] = rotate( coord_double(left, top), a, center );
  result[1] = rotate( coord_double(right, top), a, center );
  result[2] = rotate( coord_double(right, bottom), a, center );
  result[3] = rotate( coord_double(left, bottom), a, center );
} // gl_screen::get_render_coord()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the clip rectangle of a sprite in the coordinates of the OpenGL
 *        texture.
 * \param s The sprite from which we take the clip rectangle.
 */
claw::math::box_2d<GLdouble>
bear::visual::gl_screen::get_texture_clip( const sprite& s ) const
{
  const claw::math::box_2d<GLdouble> empty_clip( 0, 0, 0, 0 );
  const claw::math::rectangle<GLdouble> clip_rectangle(s.clip_rectangle());

  if ( (clip_rectangle.width == 0) || (clip_rectangle.height == 0) )
    return empty_clip;

  const claw::math::coordinate_2d<GLdouble> tex_size(s.get_image().size());

  claw::math::box_2d<GLdouble> result;

  const GLdouble half_pixel_width( 1.0 / tex_size.x / 2 );
  const GLdouble half_pixel_height( 1.0 / tex_size.y / 2 );

  result.first_point.x =
    std::max
    ( half_pixel_width,
      std::min
      ( 1.0 - half_pixel_width,
        clip_rectangle.position.x / tex_size.x ) );

  result.second_point.x =
    std::max
    ( half_pixel_width,
      std::min
      ( 1.0 - half_pixel_width,
        clip_rectangle.right() / tex_size.x ) );

  result.first_point.y =
    std::max
    ( half_pixel_height,
      std::min
      ( 1.0 - half_pixel_height,
        clip_rectangle.position.y / tex_size.y ) );

  result.second_point.y =
    std::max
    ( half_pixel_height,
      std::min
      ( 1.0 - half_pixel_height,
        clip_rectangle.bottom() / tex_size.y ) );

  if ( result.first_point.x > result.second_point.x )
    result = empty_clip;
  else if ( result.first_point.y > result.second_point.y )
    result = empty_clip;
  else
    {
      CLAW_POSTCOND( result.first_point.x > 0 );
      CLAW_POSTCOND( result.first_point.x < 1 );
      CLAW_POSTCOND( result.first_point.y > 0 );
      CLAW_POSTCOND( result.first_point.y < 1 );

      CLAW_POSTCOND( result.second_point.x > 0 );
      CLAW_POSTCOND( result.second_point.x < 1 );

      CLAW_POSTCOND( result.second_point.y > 0 );
      CLAW_POSTCOND( result.second_point.y < 1 );
    }

  return result;
} // gl_screen::get_texture_clip()

/*----------------------------------------------------------------------------*/
/**
 * \brief Draw current texture.
 * \param render_box On gl_screen position and size of the texture.
 * \param clip Part of the texture to draw.
 */
void bear::visual::gl_screen::render_image
( const claw::math::coordinate_2d<GLdouble> render_coord[],
  const claw::math::box_2d<GLdouble>& clip )
{
  glBegin(GL_QUADS);
  {
    // Top-left corner
    glTexCoord2d( clip.first_point.x, clip.first_point.y );
    glVertex3d(render_coord[0].x, render_coord[0].y, m_z_position);

    // Top-right corner
    glTexCoord2d( clip.second_point.x, clip.first_point.y );
    glVertex3d(render_coord[1].x, render_coord[1].y, m_z_position);

    // Bottom-right corner
    glTexCoord2d( clip.second_point.x, clip.second_point.y );
    glVertex3d(render_coord[2].x, render_coord[2].y, m_z_position);

    // Bottom-left corner
    glTexCoord2d( clip.first_point.x, clip.second_point.y );
    glVertex3d(render_coord[3].x, render_coord[3].y, m_z_position);
  }
  glEnd();
  VISUAL_GL_ERROR_THROW();

  update_z_position();
} // gl_screen::render_image()

/*----------------------------------------------------------------------------*/
claw::math::coordinate_2d<GLdouble> bear::visual::gl_screen::rotate
( const claw::math::coordinate_2d<GLdouble>& pos, GLdouble a,
  const claw::math::coordinate_2d<GLdouble>& center ) const
{
  claw::math::coordinate_2d<GLdouble> result(pos);
  result.rotate(center, a);
  return result;
} // gl_screen::rotate()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the size of the screen.
 * \param w The width of the screen.
 * \param h The height of the screen.
 * \param f Tell if we want a fullscreen mode.
 */
void bear::visual::gl_screen::set_video_mode
( unsigned int w, unsigned int h, bool f )
{
  const screen_size_type best_size( get_best_screen_size(w, h, f) );

  SDL_Surface* const current_surface = SDL_GetVideoSurface();
  
  if ( current_surface != NULL )
    {
      if ( ( ( (current_surface->flags & SDL_FULLSCREEN) != 0) == f )
           && (w == (unsigned int)current_surface->w)
           && (h != (unsigned int)current_surface->h) )
        return;
    }

#ifdef _WIN32
  release();
  initialize();
#endif

  Uint32 flags = SDL_OPENGL;

  if (f)
    flags |= SDL_FULLSCREEN;

  claw::logger << "Setting video mode to " << best_size.x << 'x' << best_size.y
               << ' ' << (f ? "fullscreen" : "windowed") << std::endl;

  SDL_EventState( SDL_QUIT, SDL_DISABLE );

  SDL_Surface* s = SDL_SetVideoMode( best_size.x, best_size.y, 32, flags );

  SDL_EventState( SDL_QUIT, SDL_ENABLE );

  if (!s)
    throw claw::exception( SDL_GetError() );

  m_window_size = best_size;

  delete[] m_screenshot_buffer;
  m_screenshot_buffer =
    new claw::graphic::rgba_pixel_8[ best_size.x * best_size.y ];

  SDL_ShowCursor(0);

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClearDepth(1.0);

#ifdef _WIN32
  glEnable(GL_TEXTURE_2D);
  VISUAL_GL_ERROR_THROW();

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  VISUAL_GL_ERROR_THROW();

  SDL_WM_SetCaption( m_title.c_str(), NULL );

  claw::logger << claw::log_verbose << "Screen needs restoration (was "
               << m_need_restoration << ")." << std::endl;

  m_need_restoration = true;
#endif
} // gl_screen::set_video_mode()

/*----------------------------------------------------------------------------*/
/**
 * \brief Finds a screen resolution that has is the nearest of a given size.
 * \param w The width of the screen.
 * \param h The height of the screen.
 * \param f Tell if we want a fullscreen mode.
 */
bear::visual::gl_screen::screen_size_type
bear::visual::gl_screen::get_best_screen_size
( unsigned int w, unsigned int h, bool f ) const
{
  Uint32 flags = SDL_OPENGL;

  if (f)
    flags |= SDL_FULLSCREEN;

  SDL_Rect** modes = SDL_ListModes(NULL, flags);
  screen_size_type result( w, h );

  if ( modes == NULL )
    claw::logger << claw::log_error
                 << "Cannot detect the available screen resolutions. "
                 << std::endl;
  else if ( modes == (SDL_Rect**)-1 )
    {
      claw::logger << claw::log_verbose
                   << "Any screen resolutions is accepted. "
                   << std::endl;

      const double r_x = (double)m_display_size.x / get_size().x;
      const double r_y = (double)m_display_size.y / get_size().y;
      double resize_ratio = std::min( r_x, r_y );

      if ( resize_ratio < 1 )
        result = resize_ratio * m_display_size;
    }
  else
    {
      claw::logger << claw::log_verbose << "Available screen resolutions:"
                   << std::endl;
      
      for ( std::size_t i(0); modes[i] != NULL; ++i )
        claw::logger << claw::log_verbose << modes[i]->w << 'x' << modes[i]->h
                     << std::endl;

      result = get_best_screen_size( w, h, modes );
    }

  claw::logger << claw::log_verbose << "Selected screen resolution is "
               << result.x << 'x' << result.y << '.' << std::endl;

  return result;
} // gl_screen::get_best_screen_size()

/*----------------------------------------------------------------------------*/
/**
 * \brief Finds a screen resolution that has is the nearest of a given size.
 * \param w The expected width.
 * \param h The expected height.
 * \param modes The available resolutions.
 */
bear::visual::gl_screen::screen_size_type
bear::visual::gl_screen::get_best_screen_size
( unsigned int w, unsigned int h, SDL_Rect** modes ) const
{
  const double requested_ratio( (double)w / h );
  screen_size_type result( m_display_size );
  double display_ratio( (double)m_display_size.x / m_display_size.y );
  double best_scale_factor_distance;
  
  if ( (display_ratio >= 1) && (requested_ratio >= 1) )
    best_scale_factor_distance = std::abs( (double)m_display_size.x / w - 1 );
  else
    best_scale_factor_distance = std::abs( (double)m_display_size.y / h - 1 );

  for ( std::size_t i(0); modes[i] != NULL; ++i )
    {
      const unsigned int width( modes[i]->w );
      const unsigned int height( modes[i]->h );

      const double scale_factor
        ( std::min( (double)height / h, (double)width / w ) );

      const double scale_factor_distance( std::abs( scale_factor - 1 ) );

      if ( scale_factor_distance < best_scale_factor_distance )
        {
          best_scale_factor_distance = scale_factor_distance;
          result = screen_size_type( width, height );
        }
    }

  return result;
} // gl_screen::get_best_screen_size()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tell if the user wants to close this screen.
 */
bool bear::visual::gl_screen::is_closed()
{
  SDL_PumpEvents();

  std::list<SDL_Event> not_mine;
  SDL_Event e;
  bool result( false );

  while ( !result && (SDL_PeepEvents(&e, 1, SDL_GETEVENT, SDL_ALLEVENTS) == 1) )
    if ( e.type == SDL_QUIT )
      result = true;
    else
      not_mine.push_back(e);

  for ( ; !not_mine.empty(); not_mine.pop_front() )
    SDL_PushEvent( &not_mine.front() );

  return result;
} // gl_screen::is_closed()

/*----------------------------------------------------------------------------*/
/**
 * \brief Update the z position at with the next element will be rendered.
 */
void bear::visual::gl_screen::update_z_position()
{
  m_z_position += std::numeric_limits<GLdouble>::epsilon();
} // gl_screen::update_z_position()

/*----------------------------------------------------------------------------*/
/**
 * \brief Enables a given shader.
 * \param p The shader to enable.
 */
void bear::visual::gl_screen::use_program( const shader_program& p ) const
{
  const gl_shader_program* const s
    ( static_cast<const gl_shader_program*>(p.get_impl()) );

  glUseProgram( s->program_id() );
  VISUAL_GL_ERROR_THROW();

  shader_program::variable_visitor_type visitor;
  shader_program::input_variable_map vars( p.get_variables() );
  visitor.run( vars, uniform_setter( s->program_id() ) );
} // gl_screen::use_program()
