function cats = loc_getcats(loc)

%   LOC_GETCATS -- Get the category(ies) in the locator.
%
%     See also loc_getlabs, loc_whichcat, loc_hascat
%
%     IN:
%       - `loc` (uint32) -- Locator id.
%     OUT:
%       - `cats` (uint32) -- Categories.

op_code = loc_opcodes( 'get_cats' );

cats = loc_api( op_code, loc );

end

