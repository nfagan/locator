function loc = loc_setcatmult(loc, cat, labels, indices)

%   LOC_SETCATMULT -- Assign multiple labels to category, at index
%
%     IN:
%       - `loc` (uint32) -- Locator id.
%       - `cat` (uint32) -- Category to set.
%       - `label` (uint32) -- Label to assign.
%       - `indices` (logical, uint32) -- Index identifying rows of `label`.
%     OUT:
%       - `loc` (uint32) -- Locator id.

op_code = loc_opcodes( 'set_cat_mult' );

if ( nargin < 4 )
  indices = uint32( 1:numel(labels) );
end

if ( isa(indices, 'logical') )
  indices = find( indices );
end

loc_api( op_code, loc, uint32(cat), uint32(labels), uint32(indices) );

end