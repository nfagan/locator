function cats = loc_getcats(loc)

%   LOC_GETCATS -- Get the category(ies) in the locator.
%
%     IN:
%       - `loc` (uint32) -- Locator id.
%     OUT:
%       - `cats` (uint32) -- Categories.

op_code = loc_opcodes( 'get_cats' );

cats = loc_api( op_code, loc );

end

