function loc = loc_keep(loc, indices)

%   LOC_KEEP -- Retain rows at index.
%
%     IN:
%       - `loc` (uint32) -- Locator id.
%       - `indices` (uint32, logical) -- Index of rows to keep.
%     OUT:
%       - `loc` (uint32) -- Locator id.

op_code = loc_opcodes( 'keep' );

if ( isa(indices, 'logical') )
  inds = uint32( find(indices) );
else
  inds = uint32( indices );
end

loc_api( op_code, loc, inds );

end