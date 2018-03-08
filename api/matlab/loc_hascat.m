function tf = loc_hascat(loc, cats)

%   LOC_HASCAT -- Return whether the locator has the given category(ies).
%
%     See also loc_haslab, loc_whichcat
%
%     IN:
%       - `loc` (uint32) -- Id.
%       - `cats` (uint32) -- Categories.
%     OUT:
%       - `tf` (logical)

op_code = loc_opcodes( 'has_cat' );

tf = loc_api( op_code, loc, uint32(cats) );

end