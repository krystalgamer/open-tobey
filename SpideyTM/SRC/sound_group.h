#ifndef RSG_H
#define RSG_H

#include "pstring.h"
#include "random.h"

#include "pcglobals.h"

class chunk_file;
class sg_entry

{
	friend void validate_sg_entry(void);
	friend void patch_sg_entry(void);
public:

	pstring name;
	rational_t probability;


  rational_t pitch;
  rational_t pitch_var;

  rational_t volume;
  rational_t volume_var;

  rational_t min_dist;
  rational_t max_dist;

  unsigned int field_3C;

  bool is_voice;
  unsigned int last_id_played;


  unsigned int field_48;

	bool flagged; // temporary, used by sound_group::get_sound only


  EXPORT sg_entry()
  {
    probability = 1.0f;
    pitch = 1.0f;
    volume = 1.0f;
    volume_var = 0.0f;
    pitch_var = 0.0f;
    min_dist = -1.0f;
    max_dist = -1.0f;
    is_voice = false;
    last_id_played = 0;
  }


  // @Ok
  // @Matching
  EXPORT void copy(const sg_entry &b)
  {
    name = b.name;
    probability = b.probability;
    pitch = b.pitch;
    pitch_var = b.pitch_var;
    volume = b.volume;

    volume_var = b.volume_var;

    min_dist = b.min_dist;
    max_dist = b.max_dist;

	field_3C = b.field_3C;

    is_voice = b.is_voice;

	last_id_played = b.last_id_played;

	field_48 = 0;
  }


  EXPORT sg_entry(const sg_entry& b)
  {
    copy(b);
  }

  EXPORT sg_entry& operator=(const sg_entry &b) 
  {

		copy( b );
    return *this;
  }


  EXPORT inline static rational_t variance(rational_t num, rational_t var = 0.0f)
  {
    return(num + (PLUS_MINUS_ONE * var));
  }

};

class sound_group
{
	friend void validate_sound_group(void);
	friend void patch_sound_group(void);
public:
	pstring name;
	int maxhistory;
	/*
	int history[8];
	 int history_index;
	 */
	std::vector<sg_entry> entries;
	std::vector<sg_entry*> pool;


	// Create from a text file.
	EXPORT sound_group();

   EXPORT sound_group(const sound_group &b)
  {
    copy(b);
  } 

  EXPORT void copy(const sound_group &b);

  EXPORT void clear_history();
  EXPORT void init_pool();

  EXPORT sound_group& operator=(const sound_group &b) 
  {
		copy( b );
    return *this;
  }

	// Picks the next random string and cycles the history.
	EXPORT sg_entry* get_next();

	friend void serial_in( chunk_file& fs, sound_group* group );
};

// Read a single group from a file, including the soundgrp tag.
void serial_in( chunk_file& fs, sound_group* group );


// Read a whole list from a file.  Assumes that the sound_groups: chunk has already 
// been read, and scans for a chunkend.

void serial_in( chunk_file& fs, std::vector<sound_group>& groups );

#endif
