function loc = loc_resize(loc, new_size)

%   LOC_RESIZE -- Resize the locator.
%
%     See also loc_isempty, loc_empty, loc_clear
%
%     IN:
%       - `loc` (uint32) -- Locator id.

op_code = loc_opcodes( 'resize' );

loc_api( op_code, loc, uint32(new_size) );

end