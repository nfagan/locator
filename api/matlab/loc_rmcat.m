function loc_rmcat(loc, cats)

%   LOC_RMCAT -- Remove category(ies).
%
%     IN:
%       - `loc` (uint32) -- Locator id.
%       - `cats` (uint32) -- Categories.

op_code = loc_opcodes( 'rm_cat' );

loc_api( op_code, loc, uint32(cats) );

end