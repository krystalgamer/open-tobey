#ifndef ENTITYID_H
#define ENTITYID_H
// entity id support

class charstarwrap
  {
  public:
    char* str;
    EXPORT charstarwrap(char* s) {str=s;}

    EXPORT bool operator<(const charstarwrap& csw) const

      {
      return (strcmp(str, csw.str)<0);
      }
    EXPORT bool operator==(const charstarwrap& csw) const
      {
      return (strcmp(str, csw.str)==0);
      }

  };



EXPORT char* strdupcpp(const char* str);

typedef std::map<charstarwrap,unsigned int
, std::less<charstarwrap>
  #ifdef TARGET_PS2
	, malloc_alloc

	#endif
  > name_to_number_map;

#endif
