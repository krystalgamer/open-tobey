#include "global.h"
/* Some code from Game Programming Gems
 * "Portions Copyright (C) Greg Snook, 2000"
 */
#include "ai_polypath.h"
#include "ai_polypath_cell.h"
#include "ai_polypath_heap.h"
#include "debug_render.h"
#include "wds.h"

#include "entity.h"
#include "terrain.h"

ai_polypath::ai_polypath()
  : session_id(0)
{
	cells.resize(0);
}

ai_polypath::~ai_polypath()
{
	clear();
}

void ai_polypath::clear()
{
	CELL_ARRAY::iterator	cell_i = cells.begin();
	while(cell_i != cells.end())
	{
		ai_polypath_cell* cell = *cell_i;

    ++cell_i;

		delete cell;
	}


	cells.resize(0);
}

//:	AddCell
//----------------------------------------------------------------------------------------
//
//	Add a NEW cell, defined by the three vertices in clockwise order, to this mesh
//
//-------------------------------------------------------------------------------------://
void ai_polypath::add_cell(const vector3d& a, const vector3d& b, const vector3d& c)
{
	// @TODO
	PANIC;
}




vector3d ai_polypath::snap_to_cell(ai_polypath_cell* cell, const vector3d& pt)
{
	vector3d res = pt;

	// @TODO
	PANIC;

	return (res);

}


vector3d ai_polypath::snap_to_mesh(ai_polypath_cell** cell_out, const vector3d& pt)
{

	vector3d res = pt;

	*cell_out = closest_cell(res);

	return (snap_to_cell(*cell_out, res));
}

ai_polypath_cell* ai_polypath::closest_cell(const vector3d& pt, region_node *reg, entity *ent) const
{
	// @TODO
	PANIC;
	return NULL;
}


bool ai_polypath::find_path(ai_path& path, ai_polypath_cell* start_cell, const vector3d& start_pos, ai_polypath_cell* end_cell, const vector3d& end_pos)

{
	// @TODO
	PANIC;
	return true;
}


bool ai_polypath::los_test(ai_polypath_cell* start_cell, const vector3d& start_pos, ai_polypath_cell* end_cell, const vector3d& end_pos)
{
	// @TODO
	PANIC;
	return true;
}


void ai_polypath::link()
{
	// @TODO
	PANIC;
}


void ai_polypath::optimize()
{
	// @TODO
	PANIC;
}


void ai_polypath::render()
{
#ifndef BUILD_BOOTABLE
	CELL_ARRAY::iterator cell_i = cells.begin();
	CELL_ARRAY::iterator cell_i_end = cells.end();


	while (cell_i != cell_i_end)
	{
		(*cell_i)->render();
		++cell_i;

  }
#endif
}








//#pragma todo("this is crude. need to implement better loading JDB 03-09-01")
void serial_in(chunk_file &fs, ai_polypath *path)
{
  bool warned = false;

  static std::vector<vector3d> pts;
  pts.resize(0);

  path->clear();

  stringx label;
  for(serial_in(fs, &label); label != chunkend_label && label.size() > 0; serial_in(fs, &label))
  {
    if(label == "pt")
    {
      vector3d pt;
      serial_in(fs, &pt);
      pts.push_back(pt);
    }
    else if(label == "tri")
    {
      int index[3];
      for(int i=0; i<3; ++i)

        serial_in(fs, &index[i]);


      if(index[0] < (int) pts.size() && index[1] < (int) pts.size() && index[2] < (int) pts.size() && index[0] != index[1] && index[1] != index[2] && index[2] != index[0])
      {
        vector3d ptA = (pts[index[1]] - pts[index[0]]);
        vector3d ptB = (pts[index[2]] - pts[index[0]]);
        vector3d norm = cross(ptA,ptB);

        if(norm.y < 0.0f)
        {
          if(!warned)
            warning("PolyPath '%s' has bad data, please re-export!", fs.get_filename().c_str());


          warned = true;
          int temp = index[1];
          index[1] = index[2];
          index[2] = temp;
        }

        path->add_cell(pts[index[0]], pts[index[1]], pts[index[2]]);
      }

    }
  }


  path->link();

}




void ai_path::render( camera* camera_link )
{

#ifndef BUILD_BOOTABLE

  waypoint_ID p = waypoints.begin();
  waypoint_ID p_end = waypoints.end();

  while(p != p_end)

  {
    vector3d posa = (*p).pos;
    ++p;

    render_marker(camera_link, posa, color32(255, 0, 255, 192), 0.15f);

    if(p != p_end)
    {
      vector3d posb = (*p).pos;

      render_beam(posa, posb, color32(255, 0, 0, 128), 0.05f);
    }
  }


  waypoint_ID now = waypoints.begin();
  waypoint_ID next = get_furthest_visible_waypoint(now);

  while(next != p_end)
  {

    render_beam((*now).pos, (*next).pos, color32(0, 0, 255, 128), 0.05f);

    now = next;
    next = get_furthest_visible_waypoint(now);
  }

#endif
}

bool ai_path::get_way_point( const vector3d &pos, const vector3d &lastpos, vector3d &local_dest )
{
  if(reset)
  {
    goto_start = pos;
    next_point = get_furthest_visible_waypoint(cur_point);
    reset = false;
  }

  vector3d delta = (*next_point).pos - pos;
  delta.y = 0.0f;
  vector3d move_delta = (*next_point).pos - goto_start;
  move_delta.y = 0.0f;
  if(dot(delta, move_delta) < 0.0f)
  {

    cur_point = next_point;

    next_point = get_furthest_visible_waypoint(cur_point);
    goto_start = pos;
  }

  if(next_point == waypoints.end() || cur_point == waypoints.end())
    return(false);
  else
  {
    local_dest = (*next_point).pos;
    return(true);
  }
}
