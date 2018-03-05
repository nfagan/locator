function loc_buildall()

%   LOC_BUILDALL -- Build all mex functions.

loc_build( 'loc_api.cpp' );
loc_build( 'loc_opcodes.cpp' );

end