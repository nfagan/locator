function loc = loc_collapsecat(loc, cats)

%   LOC_COLLAPSECAT -- Collapse category to single label.
%
%     See also loc_setcat, loc_whichcat
%
%     IN:
%       - `loc` (uint32) -- Locator id.
%       - `cats` (uint32) -- Category(ies) to collapse.
%     OUT:
%       - `loc` (uint32) -- Locator id.

op_code = loc_opcodes( 'collapse_cat' );

loc_api( op_code, loc, uint32(cats) );

end