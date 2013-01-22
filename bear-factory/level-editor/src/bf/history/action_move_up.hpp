/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief The action of moving selection in the top layer.
 * \author S�bastien Angibaud
 */
#ifndef __BF_ACTION_MOVE_UP_HPP__
#define __BF_ACTION_MOVE_UP_HPP__

#include "bf/history/action_group.hpp"
#include "bf/item_selection.hpp"

namespace bf
{
  /**
   * \brief The action of moving selection in the top layer.
   * \author S�bastien Angibaud
   */
  class action_move_up:
    public action_group
  {
  public:
    /**
     * \brief Constructor.
     * \param lvl The level in which we take the selection.
     */
    action_move_up( const gui_level& lvl );

    wxString get_description() const;
    void execute( gui_level& lvl );
    void undo( gui_level& lvl );

  private:
    /** \brief The new items. */
    item_selection m_new_items;

    /** \brief The previous selection. */
    item_selection m_previous_items;

    /** \brief The previous selection in the layer on the top. */
    item_selection m_previous_up_items;

    /** \brief The index of the layer in which the items are added. */
    unsigned int m_layer;
  }; // class action_move_up
} // namespace bf

#endif // __BF_ACTION_MOVE_UP_HPP__