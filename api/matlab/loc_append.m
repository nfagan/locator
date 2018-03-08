function loca = loc_append(loca, locb)

%   LOC_APPEND -- Append one locator to another.
%
%     loca = loc_append( a, b ) appends the contents of `b` to `a`.
%
%     An error is thrown if the categories of `a` do not match those of
%     `b`.
%
%     See also loc_keep, loc_create
%
%     IN:
%       - `loca` (uint32) -- Locator id a.
%       - `locb` (uint32) -- Locator id b.
%     OUT:
%       - `loca` (uint32) -- Locator id a.

op_code = loc_opcodes( 'append' );

loc_api( op_code, loca, locb );

end

