function loc_setcat(loc, cat, label, indices, rows)

%   LOC_SETCAT -- Assign label to category.
%
%     IN:
%       - `loc` (uint32) -- Locator id.
%       - `cats` (uint32) -- Category or categories to add.
%       - `indices` (logical, uint32) -- Index identifying rows of `label`.
%       - `rows` (uint32) |OPTIONAL| -- Number of rows in `indices`, if
%         `indices` is numeric and not logical.

op_code = loc_opcodes( 'set_cat' );

if ( nargin == 4 )
  inds = uint32( find(indices) );
  rows = uint32( numel(indices) );
else
  inds = indices;
end

loc_api( op_code, loc, cat, label, inds, rows );

end

