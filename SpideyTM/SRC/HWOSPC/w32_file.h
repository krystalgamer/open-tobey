#ifndef W32_FILE_H
#define W32_FILE_H

#include "..\stringx.h"


#ifndef INVALID_HANDLE_VALUE

#define INVALID_HANDLE_VALUE -1

#endif

class os_file
{
  public:
    enum mode_flags
    {
      FILE_READ   = 1,
      FILE_WRITE  = 2,
      FILE_MODIFY = 3,    // opens the file for writing, w/o deleting existing contents. (how to set new EOF? huh? --Sean)
      FILE_APPEND = 4
    };

    // ctors & dtor.
    EXPORT os_file();

    os_file(const stringx & _name, int _flags);

    ~os_file();

    void open(const stringx & _name, int _flags);
    void close();


    // read/write return number of bytes read/written.
    int read(void * data, int bytes, bool async = false);
    int write(void * data, int bytes);

    // returns file size
    int get_size(); 

    enum filepos_t

    {
      FP_BEGIN,
      FP_CURRENT,
      FP_END
    };

    // set file pointer 
    void set_fp( int pos, filepos_t base );
    unsigned int get_fp(); // relative to beginning

    // state queries

    inline const stringx & get_name() const { return name; }
    inline bool is_open() const { return opened; }           // returns true after a successful open call.
    inline bool at_eof() const;               // check this after a read operation.


    // once this is set, all os_file open operations that specify use_root are opened relative to this directory.
    static void set_root_dir(const stringx & dir);
    static const char* get_root_dir() { return root_dir; }


    // By default,the dir above root (e.g. \die2 to \die2\data)

    static void set_pre_root_dir(const stringx & dir);
    static const char* get_pre_root_dir() { return pre_root_dir; }

    // file system queries
    static bool file_exists(const stringx& name);
    static bool directory_exists(const stringx& name);

    // returns true if file1 is newer than file2.
    static bool is_file_newer(const stringx& file1, const stringx& file2);


    bool operator!() const { return false; }

    bool is_from_cd() { return from_cd; }

    enum {MAX_DIR_LEN=256};
    
  private:
    // common to all os_file implementations
    stringx name;
    int   flags;

    bool  opened;
    bool  from_cd;



    static char root_dir[MAX_DIR_LEN];
    static char pre_root_dir[MAX_DIR_LEN];

	int field_10;
	int field_14;

    
    // implementation stuff
    bool check_for_errors(int sce_error_code, char *_activity);

//    bool try_to_open_from_cd();
    bool try_to_open_from_disk(char drive_letter = 'D');


	// @Patch
    friend class movieplayer;

	friend void validate_os_file();
};            

// @Patch
typedef os_file* host_system_file_handle;

#endif
