function cats = loc_whichcat(loc, labels)

%   LOC_WHICHCAT -- Get the category(ies) in which label(s) reside.
%
%     See also loc_hascat, loc_find
%
%     IN:
%       - `loc` (uint32) -- Locator id.
%       - `labels` (uint32) -- Labels to search.
%     OUT:
%       - `cats` (uint32) -- Categories.

op_code = loc_opcodes( 'which_cat' );

cats = loc_api( op_code, loc, uint32(labels) );

end

