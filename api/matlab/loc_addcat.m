function loc_addcat( loc, cats )

%   LOC_ADDCAT -- Add category(ies) to the locator.
%
%     IN:
%       - `loc` (uint32) -- Locator id.
%       - `cats` (uint32) -- Category or categories to add.

op_code = loc_opcodes( 'add_cat' );

loc_api( op_code, loc, uint32(cats) );

end

