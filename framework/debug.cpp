#include "include/public/toon_boom/ext/util.hpp"

	std::ostream devnull(new NullBuffer());
	
	std::ostream& out = TB_EXT_FRAMEWORK_DEBUG ? std::cout : devnull;	
