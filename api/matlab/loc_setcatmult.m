function loc = loc_setcatmult(loc, cat, labels, indices)

%   LOC_SETCATMULT -- Assign multiple labels to category, at index.
%
%     See also loc_setcat
%
%     IN:
%       - `loc` (uint32) -- Locator id.
%       - `cat` (uint32) -- Category to set.
%       - `label` (uint32) -- Label to assign.
%       - `indices` (logical, uint32) -- Index identifying rows of `label`.
%     OUT:
%       - `loc` (uint32) -- Locator id.

op_code = loc_opcodes( 'set_cat' );

labels = uint32( labels(:) );

sz = loc_size( loc );

if ( sz == 0 )
  sz = uint32( numel(labels) );
end

if ( nargin < 4 )
  indices = true( numel(labels), 1 );
else
  if ( ~isa(indices, 'logical') )
    inds = true( sz, 1 );
    inds(indices) = true;
    indices = inds;
  end
end

unqs = unique( labels );

cat = uint32( cat );

for i = 1:numel(unqs)
  lab = unqs(i);
  ind = labels == lab & indices;
  index = uint32( find(ind) );
  loc_api( op_code, loc, cat, lab, index, sz );
end

end