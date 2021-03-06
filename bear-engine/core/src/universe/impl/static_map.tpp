/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the bear::universe::static_map class.
 * \author Julien Jorge.
 */

#include <claw/assert.hpp>
#include <claw/logger.hpp>
#include <limits>
#include <set>

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 * \param width Width of the whole map.
 * \param height Height of the whole map.
 * \param box_size Size of the boxes.
 *
 * \remark Big values for box_size increase the compression but also the work
 *         done by get_area(). In an other hand, small values decrease the
 *         work but also the compression. You should find a good compromize
 *         (the empty_cells() and cells_load() methods are made to help you).
 */
template<class ItemType>
bear::universe::static_map<ItemType>::static_map
( unsigned int width, unsigned int height, unsigned int box_size )
  : m_box_size(box_size),
    m_size(width / m_box_size + 1, height / m_box_size + 1),
    m_map( m_size.x, column(m_size.y) )
{
  CLAW_PRECOND( width > 0 );
  CLAW_PRECOND( height > 0 );

  // in fact, floating point exception had already been thrown
  CLAW_PRECOND( box_size > 0 );
} // static_map::static_map()

/*----------------------------------------------------------------------------*/
/**
 * \brief Add an item in the map.
 * \param item The item to add.
 */
template<class ItemType>
void bear::universe::static_map<ItemType>::insert( const item_type& item )
{
  const area_type box = item->get_bounding_box();
  int top = (int)box.top() / (int)m_box_size;
  int left = (int)box.left() / (int)m_box_size;
  int bottom = (int)box.bottom() / (int)m_box_size;
  int right = (int)box.right() / (int)m_box_size;

  if ( (top < 0) || (bottom >= (int)m_size.y)
       || (right < 0) || (left >= (int)m_size.x) )
    claw::logger << claw::log_warning
                 << "Item is outside the map. Its position in the map is ("
                 << left << ' ' << bottom << ' ' << right << ' ' << top << ' '
                 << "), its real position is ("
                 << box.left() << ' ' << box.bottom() << ' ' << box.right()
                 << ' ' << box.top() << ")." << std::endl;

  if ( top >= (int)m_size.y )
    top = m_size.y - 1;
  if ( bottom < 0 )
    bottom = 0;
  if ( right >= (int)m_size.x )
    right = m_size.x - 1;
  if ( left < 0 )
    left = 0;

  for ( int col = left; col <= right; ++col )
    for ( int line = bottom; line <= top; ++line )
      m_map[col][line].push_front(item);
} // static_map::insert()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get all items inside rectangular regions of the map, without
 *        duplicates.
 * \param first Iterator on the first area from which to take the items.
 * \param last Iterator just past the last the first area from which to take the
 *        items.
 * \param items (in/out) The items found.
 */
template<class ItemType>
template<typename AreaIterator>
void bear::universe::static_map<ItemType>::get_areas_unique
( AreaIterator first, AreaIterator last, item_list& items ) const
{
  get_areas(first, last, items);
  make_set(items);
} // static_map::get_areas_unique()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get all items inside rectangular regions of the map.
 * \param first Iterator on the first area from which to take the items.
 * \param last Iterator just past the last the first area from which to take the
 *        items.
 * \param items (in/out) The items found.
 */
template<class ItemType>
template<typename AreaIterator>
void bear::universe::static_map<ItemType>::get_areas
( AreaIterator first, AreaIterator last, item_list& items ) const
{
  for ( ; first!=last; ++first )
    get_area( *first, items );
} // static_map::get_areas()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get all items inside a rectangular region of the map.
 * \param area The area from which to take the items.
 * \param items (out) The items found.
 */
template<class ItemType>
void bear::universe::static_map<ItemType>::get_area_unique
( const area_type& area, item_list& items ) const
{
  item_list result;
  get_area( area, result );
  make_set( result );
  items.splice( items.end(), result );
} // static_map::get_area()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get all items inside a rectangular region of the map.
 * \param area The area from which to take the items.
 * \param items (out) The items found.
 */
template<class ItemType>
void bear::universe::static_map<ItemType>::get_area
( const area_type& area, item_list& items ) const
{
  unsigned int x, y;
  typename item_box::const_iterator it;

  unsigned int min_x = (unsigned int)area.left() / m_box_size;
  unsigned int max_x = (unsigned int)area.right() / m_box_size;
  unsigned int min_y = (unsigned int)area.bottom() / m_box_size;
  unsigned int max_y = (unsigned int)area.top() / m_box_size;

  if ( max_x >= m_size.x )
    max_x = m_size.x - 1;

  if ( max_y >= m_size.y )
    max_y = m_size.y - 1;

  for (x=min_x; x<=max_x; ++x)
    for (y=min_y; y<=max_y; ++y)
      for (it=m_map[x][y].begin(); it!=m_map[x][y].end(); ++it)
        if ( (*it)->get_bounding_box().intersects(area) )
          items.push_back( *it );
} // static_map::get_area()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get all items.
 * \param items (out) The items found.
 */
template<class ItemType>
void
bear::universe::static_map<ItemType>::get_all_unique( item_list& items ) const
{
  item_list result;
  unsigned int x, y;
  typename item_box::const_iterator it;

  for (x=0; x!=m_map.size(); ++x)
    for (y=0; y!=m_map[x].size(); ++y)
      result.insert( result.end(), m_map[x][y].begin(), m_map[x][y].end() );

  make_set(result);
  items.splice( items.end(), result );
} // static_map::get_all()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tell the number of empty cells in the compressed map.
 */
template<class ItemType>
unsigned int bear::universe::static_map<ItemType>::empty_cells() const
{
  unsigned int cells=0;
  unsigned int x, y;

  for (x=0; x!=m_map.size(); ++x)
    for (y=0; y!=m_map[x].size(); ++y)
      if ( m_map[x][y].empty() )
        ++cells;

  return cells;
} // static_map::empty_cells()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get some statistics about the cells content.
 * \param min (out) Minimum number of items found in a cell.
 * \param max (out) Maximum number of items found in a cell.
 * \param avg (out) Average number of items found in not empty cells.
 */
template<class ItemType>
void bear::universe::static_map<ItemType>::cells_load
( unsigned int& min, unsigned int& max, double& avg ) const
{
  unsigned int not_empty_cells=0;
  unsigned int x, y;
  unsigned int load=0;

  min = std::numeric_limits<unsigned int>::max();
  max = 0;
  avg = 0;

  for (x=0; x!=m_map.size(); ++x)
    for (y=0; y!=m_map[x].size(); ++y)
      {
        const std::size_t size( m_map[x][y].size() );

        if ( size > max )
          max = size;

        if ( size < min )
          min = size;

        if (size != 0)
          {
            load += size;
            ++not_empty_cells;
          }
      }

  if ( (load != 0) && (not_empty_cells!=0) )
    avg = (double)load / (double)not_empty_cells;
} // static_map::cells_load()

/*----------------------------------------------------------------------------*/
/**
 * \brief Remove duplicate items in a list of items.
 * \param items The list from which we make a set.
 */
template<class ItemType>
void bear::universe::static_map<ItemType>::make_set
( item_list& items ) const
{
  item_list result;
  std::set<ItemType> seen;

  for ( ; !items.empty(); items.pop_front() )
    if ( seen.insert(items.front()).second )
      result.push_back(items.front());

  items.swap(result);
} // static_map::make_set()
