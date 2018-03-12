function loc_testall()

%   LOC_TESTALL -- Run all tests.

outer_dir = fileparts( which(mfilename) );

test_dir = fullfile( outer_dir, 'test' );

addpath( test_dir );

loc_test_run( @loc_test_fullcat );
loc_test_run( @loc_test_resize );
loc_test_run( @loc_test_append_random );
loc_test_run( @loc_test_pipeline );
loc_test_run( @loc_test_from );
loc_test_run( @loc_test_keep );
loc_test_run( @loc_test_keep_one );
loc_test_run( @loc_test_struct );
loc_test_run( @loc_test_eq );
loc_test_run( @loc_test_append );
loc_test_run( @loc_test_eq_several );
loc_test_run( @loc_test_combs );

end