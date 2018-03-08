function tf = loc_eq(loca, locb)

%   LOC_EQ -- True if locators a and b have equal contents.
%
%     See also loc_create, loc_getlabs, loc_find
%
%     IN:
%       - `loca` (uint32) -- Locator id a.
%       - `locb` (uint32) -- Locator id b.
%     OUT:
%       - `tf` (logical)

op_code = loc_opcodes( 'eq' );

tf = loc_api( op_code, loca, locb );

end

