/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief Implementation of the bf::action_align_selection_top class.
 * \author Julien Jorge
 */
#include "bf/history/action_align_selection_top.hpp"

#include "bf/item_instance.hpp"
#include "bf/item_selection.hpp"
#include "bf/history/action_set_item_bottom.hpp"

#include <wx/intl.h>

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 * \param selection The items to align.
 */
bf::action_align_selection_top::action_align_selection_top
( const item_selection& selection )
  : action_group( _("Align the selected items") )
{
  if ( selection.empty() )
    return;

  const item_instance& main_item( *selection.get_main_selection() );

  for ( item_selection::const_iterator it( selection.begin() );
        it != selection.end(); ++it)
    add_action
      ( new action_set_item_bottom
        ( *it,
          main_item.get_rendering_parameters().get_bottom()
          + main_item.get_rendering_parameters().get_height()
          - (*it)->get_rendering_parameters().get_height() ) );
} // action_align_selection_top::action_align_selection_top()
