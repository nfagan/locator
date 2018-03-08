function sz = loc_size(loc)

%   LOC_SIZE -- Get the number of rows in the locator.
%
%     See also loc_isempty, loc_empty, loc_clear
%
%     IN:
%       - `loc` (uint32) -- Locator id.
%     OUT:
%       - `sz` (uint32) -- Number of rows in `loc`.

op_code = loc_opcodes( 'size' );

sz = loc_api( op_code, loc );

end