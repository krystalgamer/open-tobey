// Localization support.
#ifndef LOCALIZE_H
#define LOCALIZE_H

#include "stringx.h"
#include "singleton.h"
#include <map>


void load_locales();
stringx localize_text( stringx src );
stringx localize_text_safe( stringx src );

// localize a VO stream filename
stringx localize_VO_stream( const stringx& filename );

// @Patch
class localize_manager : public singleton
{
	friend void patch_localize_manager(void);
	friend void validate_localize_manager(void);

	private:
	// @Patch - assume kv type and name
		std::map<stringx, stringx> unk;

	public:
		EXPORT int get_language_id(void) const;

};

#endif
