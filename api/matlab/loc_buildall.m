function loc_buildall()

%   LOC_BUILDALL -- Build all mex functions.

loc_build( {'loc_api.cpp', 'loc_mex_helpers.cpp'} );
loc_build( 'loc_opcodes.cpp' );

loc_build( {'loc_multimap_api.cpp', 'loc_mex_helpers.cpp'} );
loc_build( 'loc_multimap_opcodes.cpp' );

end