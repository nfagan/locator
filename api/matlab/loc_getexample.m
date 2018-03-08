function [loc, data] = loc_getexample()

%   LOC_GETEXAMPLE -- Get an example locator.
%
%     OUT:
%       - `loc` (uint32) -- Handle to newly constructed locator.
%       - `data` (struct) -- Struct input used to build `loc`.

outer_dir = fileparts( which(mfilename) );
example_file = fullfile( outer_dir, 'data', 'example_struct.mat' );

if ( exist(example_file, 'file') ~= 2 )
  error( 'Example file, expected to be stored in api/matlab/data, does not exist' );
end

data = load( example_file );

data = data.(char(fieldnames(data)));

loc = loc_from( data );

end