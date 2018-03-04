function loc_requirecat(loc, cats)

%   LOC_REQUIRECAT -- Add category(ies) if they do not already exist.
%
%     IN:
%       - `loc` (uint32) -- Locator id.
%       - `cats` (uint32) -- Categories.

op_code = loc_opcodes( 'require_cat' );

loc_api( op_code, loc, uint32(cats) );

end