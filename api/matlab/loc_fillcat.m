function loc = loc_fillcat(loc, cat, label)

%   LOC_FILLCAT -- Assign full contents of category to label.
%
%     IN:
%       - `loc` (uint32) -- Locator id.
%       - `cat` (uint32) -- Category to set.
%       - `label` (uint32) -- Label to assign.
%     OUT:
%       - `loc` (uint32) -- Locator id.

op_code = loc_opcodes( 'fill_cat' );

loc_api( op_code, loc, uint32(cat), uint32(label) );

end