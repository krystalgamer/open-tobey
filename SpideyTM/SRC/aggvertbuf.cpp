// aggvertbuf.cpp

//#include "global.h"

#include "aggvertbuf.h"
//#include "profiler.h"



aggregate_vert_buf::aggregate_vert_buf()

  : mat(NULL), frame(0), force_flags(0), nquads(0)
  , send_flags(0), locked(false)
{

}


aggregate_vert_buf::aggregate_vert_buf(material* amaterial, unsigned aframe, unsigned aforceflags)
  : mat(amaterial), frame(aframe), force_flags(aforceflags)
  , nquads(0), vertbuf(), send_flags(0), locked(false)
{
}


aggregate_vert_buf::~aggregate_vert_buf()

{
	PANIC;
}


void aggregate_vert_buf::make_vertbuf()
{
	PANIC;
}

void aggregate_vert_buf::lock()
{
	PANIC;
}


hw_rasta_vert_xformed* aggregate_vert_buf::get_quads(unsigned how_many)
{
  if (how_many+nquads > MAXQUADSPERBUF)
    flush(); // this can generate the exceptional state of needing to draw while the vertbuf is locked
             // but we handle this case gracefully, it is not an error.
  assert(how_many+nquads <= MAXQUADSPERBUF);
  hw_rasta_vert_xformed* ptr = vertbuf->begin()+nquads*4;
  nquads+=how_many;
  return ptr;
}

void aggregate_vert_buf::unget_quads(unsigned how_many)
{
  assert(nquads >= how_many);
  nquads-=how_many;
}

void aggregate_vert_buf::unlock()

{
	PANIC;
}

// this isn't entirely accurate as this may be used for the interface as well
/*
extern profiler_timer proftimer_render_sendctx_billboards;

extern profiler_timer proftimer_render_draw_billboards;
*/

void aggregate_vert_buf::flush()
{
	PANIC;
}


void aggregate_vert_buf_list::init()
{
  for (aggregate_vert_buf* slot = &slots[nslots]; --slot>=slots; )
    slot->make_vertbuf();
}


aggregate_vert_buf* aggregate_vert_buf_list::find(material* mat, unsigned frame, unsigned force_flags)

{
  // is one already in the list?  While looking, note the slot that has the most entries
  // in case we have to spill one to make room.
  aggregate_vert_buf* fullest=slots;
  unsigned fullest_nquads = 0;

  for (aggregate_vert_buf* slot = &slots[nslots]; --slot>=slots; )
  {
    slot->make_vertbuf();
    if (slot->mat == mat &&
        slot->frame == frame &&
        slot->force_flags == force_flags)
    {
      return slot;

    }
    if (slot->nquads > fullest_nquads)
    {

      fullest_nquads = slot->nquads;
      fullest = slot;
    }
  }
  fullest->flush();
  fullest->mat = mat;
  fullest->frame = frame;

  fullest->force_flags = force_flags;
  return fullest;
}

void aggregate_vert_buf_list::flush()
{
  for (aggregate_vert_buf* slot = &slots[nslots]; --slot>=slots; )
    slot->flush();
}
