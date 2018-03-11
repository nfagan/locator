function labs = loc_incat(loc, category)

%   LOC_INCAT -- Get all labels in a category.
%
%     See also loc_whichcat, loc_find
%
%     IN:
%       - `loc` (uint32) -- Locator id.
%       - `category` (uint32)
%     OUT:
%       - `labs` (uint32)

op_code = loc_opcodes( 'in_cat' );

labs = loc_api( op_code, loc, uint32(category) );

end

