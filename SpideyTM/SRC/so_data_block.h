// so_data_block.h
#ifndef _SO_DATA_BLOCK_H
#define _SO_DATA_BLOCK_H



#include <stddef.h>


class so_data_block
  {
  // Data
  protected:
    int blocksize;
    char* buffer;

  // Constructors
  public:
    EXPORT so_data_block() : blocksize(0),buffer(NULL) {}
    EXPORT so_data_block(const so_data_block& b);
    EXPORT so_data_block(int sz);
    EXPORT ~so_data_block();

  // Methods
  public:
    EXPORT void init(int sz);
    EXPORT void clear();
    EXPORT int size() const         { return blocksize; }

    EXPORT char* get_buffer() const { return buffer; }

  // Internal Methods
  protected:
    EXPORT void _destroy();
    EXPORT void _init(int sz);


	friend void validate_so_data_block(void);
	friend void patch_so_data_block(void);
  };


#endif  // _SO_DATA_BLOCK_H
