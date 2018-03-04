function indices = loc_find(loc, labels)

%   LOC_FIND -- Get indices associated with labels.
%
%     IN:
%       - `loc` (uint32) -- Locator id.
%       - `labels` (uint32) -- Labels
%     OUT:
%       - `indices` (uint32) -- Index of elements associated with `labels`.

op_code = loc_opcodes( 'find' );

indices = loc_api( op_code, loc, uint32(labels) );

end

