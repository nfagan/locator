function loc = loc_create()

%   LOC_CREATE -- Create a new locator.
%
%     OUT:
%       - `loc` (uint32) -- Handle to the created locator.

op_code = loc_opcodes( 'create' );

loc = loc_api( op_code );

end