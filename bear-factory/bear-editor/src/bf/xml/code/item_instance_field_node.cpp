/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief Implementation of the bf::xml::item_instance_field_node class.
 * \author Julien Jorge
 */
#include "bf/xml/item_instance_field_node.hpp"

#include "bf/human_readable.hpp"
#include "bf/item_class.hpp"
#include "bf/item_instance.hpp"
#include "bf/xml/reader_tool.hpp"
#include "bf/xml/value_to_xml.hpp"
#include "bf/xml/xml_to_value.hpp"
#include "bf/wx_facilities.hpp"

#include <claw/assert.hpp>
#include <claw/logger.hpp>

/*----------------------------------------------------------------------------*/
/**
 * \brief Read an xml node "field".
 * \param item (out) The item class for we read the hierarchy.
 * \param node The node.
 */
void bf::xml::item_instance_field_node::read
( item_instance& item, const wxXmlNode* node ) const
{
  CLAW_PRECOND( node!=NULL );
  CLAW_PRECOND( node->GetName() == wxT("field") );

  const std::string field_name( reader_tool::read_string(node, wxT("name")) );
  const item_class& the_class(item.get_class());

  if ( !the_class.has_field(field_name) )
    claw::logger << claw::log_warning << "Unknown field '" << field_name
                 << "' in '" << the_class.get_class_name() << "'" << std::endl;
  else
    {
      const type_field& field = the_class.get_field(field_name);
      load_field( item, field, node->GetChildren() );
    }
} // item_instance_field_node::read()

/*----------------------------------------------------------------------------*/
/**
 * \brief Write an xml node "field".
 * \param item The item instance to write.
 * \param f The field to save.
 * \param os The stream in which we write.
 */
void bf::xml::item_instance_field_node::write
( const item_instance& item, const type_field& f, std::ostream& os ) const
{
  os << "<field name='" << f.get_name() << "'>\n";
  save_field( item, f, os );
  os << "</field>\n";
} // item_instance_field_node::write()

/*----------------------------------------------------------------------------*/
/**
 * \brief Load the value of a field.
 * \param item The item in which we set the field.
 * \param f The field to load.
 * \param node The node to parse.
 */
void bf::xml::item_instance_field_node::load_field
( item_instance& item, const type_field& f, const wxXmlNode* node ) const
{
  if ( f.is_list() )
    switch ( f.get_field_type() )
      {
      case type_field::integer_field_type:
        load_value_list<integer_type>( item, f.get_name(), node );
        break;
      case type_field::u_integer_field_type:
        load_value_list<u_integer_type>( item, f.get_name(), node );
        break;
      case type_field::real_field_type:
        load_value_list<real_type>( item, f.get_name(), node );
        break;
      case type_field::boolean_field_type:
        load_value_list<bool_type>( item, f.get_name(), node );
        break;
      case type_field::string_field_type:
        load_value_list<string_type>( item, f.get_name(), node );
        break;
      case type_field::sprite_field_type:
        load_value_list<sprite>( item, f.get_name(), node );
        break;
      case type_field::animation_field_type:
        load_value_list<any_animation>( item, f.get_name(), node );
        break;
      case type_field::item_reference_field_type:
        load_value_list<item_reference_type>( item, f.get_name(), node );
        break;
      case type_field::font_field_type:
        load_value_list<font>( item, f.get_name(), node );
        break;
      case type_field::sample_field_type:
        load_value_list<sample>( item, f.get_name(), node );
        break;
      case type_field::color_field_type:
        load_value_list<color>( item, f.get_name(), node );
        break;
      }
  else
    switch ( f.get_field_type() )
      {
      case type_field::integer_field_type:
        load_value<integer_type>( item, f.get_name(), node );
        break;
      case type_field::u_integer_field_type:
        load_value<u_integer_type>( item, f.get_name(), node );
        break;
      case type_field::real_field_type:
        load_value<real_type>( item, f.get_name(), node );
        break;
      case type_field::boolean_field_type:
        load_value<bool_type>( item, f.get_name(), node );
        break;
      case type_field::string_field_type:
        load_value<string_type>( item, f.get_name(), node );
        break;
      case type_field::sprite_field_type:
        load_value<sprite>( item, f.get_name(), node );
        break;
      case type_field::animation_field_type:
        load_value<any_animation>( item, f.get_name(), node );
        break;
      case type_field::item_reference_field_type:
        load_value<item_reference_type>( item, f.get_name(), node );
        break;
      case type_field::font_field_type:
        load_value<font>( item, f.get_name(), node );
        break;
      case type_field::sample_field_type:
        load_value<sample>( item, f.get_name(), node );
        break; 
      case type_field::color_field_type:
        load_value<color>( item, f.get_name(), node );
        break;
      }
} // item_instance_field_node::load_field()

/*----------------------------------------------------------------------------*/
/**
 * \brief Save the value of a field.
 * \param item The item in which we take the field.
 * \param f The field to save.
 * \param os The stream in which we write.
 */
void bf::xml::item_instance_field_node::save_field
( const item_instance& item, const type_field& f, std::ostream& os ) const
{
  if ( f.is_list() )
    switch ( f.get_field_type() )
      {
      case type_field::integer_field_type:
        save_value_list<integer_type>( os, f.get_name(), item, "integer" );
        break;
      case type_field::u_integer_field_type:
        save_value_list<u_integer_type>( os, f.get_name(), item, "u_integer" );
        break;
      case type_field::real_field_type:
        save_value_list<real_type>( os, f.get_name(), item, "real" );
        break;
      case type_field::boolean_field_type:
        save_value_list<bool_type>( os, f.get_name(), item, "bool" );
        break;
      case type_field::string_field_type:
        save_value_list<string_type>( os, f.get_name(), item, "string" );
        break;
      case type_field::sprite_field_type:
        save_sprite_list( os, f.get_name(), item );
        break;
      case type_field::animation_field_type:
        save_animation_list( os, f.get_name(), item );
        break;
      case type_field::item_reference_field_type:
        save_value_list<item_reference_type>
          ( os, f.get_name(), item, "item_reference" );
        break;
      case type_field::font_field_type:
        save_font_list( os, f.get_name(), item );
        break;
      case type_field::sample_field_type:
        save_sample_list( os, f.get_name(), item );
        break;
      case type_field::color_field_type:
        save_color_list( os, f.get_name(), item );
        break;
      }
  else
    switch ( f.get_field_type() )
      {
      case type_field::integer_field_type:
        save_value<integer_type>( os, f.get_name(), item, "integer" );
        break;
      case type_field::u_integer_field_type:
        save_value<u_integer_type>( os, f.get_name(), item, "u_integer" );
        break;
      case type_field::real_field_type:
        save_value<real_type>( os, f.get_name(), item, "real" );
        break;
      case type_field::boolean_field_type:
        save_value<bool_type>( os, f.get_name(), item, "bool" );
        break;
      case type_field::string_field_type:
        save_value<string_type>( os, f.get_name(), item, "string" );
        break;
      case type_field::sprite_field_type:
        save_sprite( os, f.get_name(), item );
        break;
      case type_field::animation_field_type:
        save_animation( os, f.get_name(), item );
        break;
      case type_field::item_reference_field_type:
        save_value<item_reference_type>
          ( os, f.get_name(), item, "item_reference" );
        break;
      case type_field::font_field_type:
        save_font( os, f.get_name(), item );
        break;
      case type_field::sample_field_type:
        save_sample( os, f.get_name(), item );
        break;
      case type_field::color_field_type:
        save_color( os, f.get_name(), item );
        break;
      }
} // item_instance_field_node::save_field()

/*----------------------------------------------------------------------------*/
/**
 * \brief Save the value of a field of type 'sprite'.
 * \param os The stream in which we save the value.
 * \param field_name The name of the field to save.
 * \param item The item in which we take the value.
 */
void bf::xml::item_instance_field_node::save_sprite
( std::ostream& os, const std::string& field_name,
  const item_instance& item ) const
{
  sprite spr;
  item.get_value( field_name, spr );
  xml::value_to_xml<sprite>::write(os, spr);
} // item_instance_field_node::save_sprite()

/*----------------------------------------------------------------------------*/
/**
 * \brief Save the value of a field of type 'animation'.
 * \param os The stream in which we save the value.
 * \param field_name The name of the field to save.
 * \param item The item in which we take the value.
 */
void bf::xml::item_instance_field_node::save_animation
( std::ostream& os, const std::string& field_name,
  const item_instance& item ) const
{
  any_animation anim;
  item.get_value( field_name, anim );
  xml::value_to_xml<any_animation>::write(os, anim);
} // item_instance_field_node::save_animation()

/*----------------------------------------------------------------------------*/
/**
 * \brief Save the value of a field of type 'font'.
 * \param os The stream in which we save the value.
 * \param field_name The name of the field to save.
 * \param item The item in which we take the value.
 */
void bf::xml::item_instance_field_node::save_font
( std::ostream& os, const std::string& field_name,
  const item_instance& item ) const
{
  font f;
  item.get_value( field_name, f );
  xml::value_to_xml<font>::write(os, f);
} // item_instance_field_node::save_font()

/*----------------------------------------------------------------------------*/
/**
 * \brief Save the value of a field of type 'color'.
 * \param os The stream in which we save the value.
 * \param field_name The name of the field to save.
 * \param item The item in which we take the value.
 */
void bf::xml::item_instance_field_node::save_color
( std::ostream& os, const std::string& field_name,
  const item_instance& item ) const
{
  color f;
  item.get_value( field_name, f );
  xml::value_to_xml<color>::write(os, f);
} // item_instance_field_node::save_color()

/*----------------------------------------------------------------------------*/
/**
 * \brief Save the value of a field of type 'sample'.
 * \param os The stream in which we save the value.
 * \param field_name The name of the field to save.
 * \param item The item in which we take the value.
 */
void bf::xml::item_instance_field_node::save_sample
( std::ostream& os, const std::string& field_name,
  const item_instance& item ) const
{
  sample s;
  item.get_value( field_name, s );
  xml::value_to_xml<sample>::write(os, s);
} // item_instance_field_node::save_sample()

/*----------------------------------------------------------------------------*/
/**
 * \brief Save the value of a field of type 'list of sprite'.
 * \param os The stream in which we save the value.
 * \param field_name The name of the field to save.
 * \param item The item in which we take the value.
 */
void bf::xml::item_instance_field_node::save_sprite_list
( std::ostream& os, const std::string& field_name,
  const item_instance& item ) const
{
  std::list<sprite> spr;
  std::list<sprite>::const_iterator it;

  item.get_value( field_name, spr );

  for (it=spr.begin(); it!=spr.end(); ++it)
    xml::value_to_xml<sprite>::write(os, *it);
} // item_instance_field_node::save_sprite_list()

/*----------------------------------------------------------------------------*/
/**
 * \brief Save the value of a field of type 'list of animation'.
 * \param os The stream in which we save the value.
 * \param field_name The name of the field to save.
 * \param item The item in which we take the value.
 */
void bf::xml::item_instance_field_node::save_animation_list
( std::ostream& os, const std::string& field_name,
  const item_instance& item ) const
{
  std::list<any_animation> anim;
  std::list<any_animation>::const_iterator it;

  item.get_value( field_name, anim );

  for (it=anim.begin(); it!=anim.end(); ++it)
    xml::value_to_xml<any_animation>::write(os, *it);
} // item_instance_field_node::save_animation_list()

/*----------------------------------------------------------------------------*/
/**
 * \brief Save the value of a field of type 'list of font'.
 * \param os The stream in which we save the value.
 * \param field_name The name of the field to save.
 * \param item The item in which we take the value.
 */
void bf::xml::item_instance_field_node::save_font_list
( std::ostream& os, const std::string& field_name,
  const item_instance& item ) const
{
  std::list<font> f;
  std::list<font>::const_iterator it;

  item.get_value( field_name, f );

  for (it=f.begin(); it!=f.end(); ++it)
    xml::value_to_xml<font>::write(os, *it);
} // item_instance_field_node::save_font_list()

/*----------------------------------------------------------------------------*/
/**
 * \brief Save the value of a field of type 'list of color'.
 * \param os The stream in which we save the value.
 * \param field_name The name of the field to save.
 * \param item The item in which we take the value.
 */
void bf::xml::item_instance_field_node::save_color_list
( std::ostream& os, const std::string& field_name,
  const item_instance& item ) const
{
  std::list<color> f;
  std::list<color>::const_iterator it;

  item.get_value( field_name, f );

  for (it=f.begin(); it!=f.end(); ++it)
    xml::value_to_xml<color>::write(os, *it);
} // item_instance_field_node::save_color_list()

/*----------------------------------------------------------------------------*/
/**
 * \brief Save the value of a field of type 'list of sample'.
 * \param os The stream in which we save the value.
 * \param field_name The name of the field to save.
 * \param item The item in which we take the value.
 */
void bf::xml::item_instance_field_node::save_sample_list
( std::ostream& os, const std::string& field_name,
  const item_instance& item ) const
{
  std::list<sample> s;
  std::list<sample>::const_iterator it;

  item.get_value( field_name, s );

  for (it=s.begin(); it!=s.end(); ++it)
    xml::value_to_xml<sample>::write(os, *it);
} // item_instance_field_node::save_sample_list()

/*----------------------------------------------------------------------------*/
/**
 * \brief Load the value of a field of simple type.
 * \param item The item in which we save the value.
 * \param field_name The name of the field to load.
 * \param node The node to parse.
 */
template<typename Type>
void bf::xml::item_instance_field_node::load_value
( item_instance& item, const std::string& field_name,
  const wxXmlNode* node ) const
{
  Type v;

  node = reader_tool::skip_comments(node);

  if ( node == NULL )
    throw xml::missing_node( "Content for field '" + field_name  + '\'' );

  xml::xml_to_value<Type> xml_conv;
  xml_conv( v, node );

  if ( wx_to_std_string( human_readable<Type>::convert(v) )
       != item.get_class().get_default_value(field_name) )
    item.set_value( field_name, v );
} // item_instance_field_node::load_value()

/*----------------------------------------------------------------------------*/
/**
 * \brief Load the value of a field of type list of simple type.
 * \param item The item in which we save the value.
 * \param field_name The name of the field to load.
 * \param node The node to parse.
 */
template<typename Type>
void bf::xml::item_instance_field_node::load_value_list
( item_instance& item, const std::string& field_name,
  const wxXmlNode* node ) const
{
  std::list<Type> v;

  xml::xml_to_value<Type> xml_conv;

  node = reader_tool::skip_comments(node);

  while ( node!=NULL )
    {
      Type tmp;
      xml_conv( tmp, node );
      v.push_back(tmp);

      node = reader_tool::skip_comments(node->GetNext());
    }

  item.set_value( field_name, v );
} // item_instance_field_node::load_value_list()

/*----------------------------------------------------------------------------*/
/**
 * \brief Save the value of a field of simple type.
 * \param os The stream in which we save the value.
 * \param field_name The name of the field to save.
 * \param item The item in which we take the value.
 * \param node_name The name of the xml node.
 */
template<typename Type>
void bf::xml::item_instance_field_node::save_value
( std::ostream& os, const std::string& field_name,
  const item_instance& item, const std::string& node_name ) const
{
  Type v;
  item.get_value( field_name, v );
  xml::value_to_xml<Type>::write( os, node_name, v );
} // item_instance_field_node::save_value()

/*----------------------------------------------------------------------------*/
/**
 * \brief Save the value of a field of list of simple type.
 * \param os The stream in which we save the value.
 * \param field_name The name of the field to save.
 * \param item The item in which we take the value.
 * \param node_name The name of the xml node.
 */
template<typename Type>
void bf::xml::item_instance_field_node::save_value_list
( std::ostream& os, const std::string& field_name,
  const item_instance& item, const std::string& node_name ) const
{
  std::list<Type> v;
  typename std::list<Type>::const_iterator it;

  item.get_value( field_name, v );

  for (it=v.begin(); it!=v.end(); ++it)
    xml::value_to_xml<Type>::write( os, node_name, *it );
} // item_instance_field_node::save_value_list()