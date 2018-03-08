function loc_testall()

outer_dir = fileparts( which(mfilename) );

test_dir = fullfile( outer_dir, 'test' );

addpath( test_dir );

%
%   run
%

loc_test_pipeline();
loc_test_from();
loc_test_keep();
loc_test_struct();
loc_test_eq();

end