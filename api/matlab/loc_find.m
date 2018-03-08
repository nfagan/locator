function indices = loc_find(loc, labels)

%   LOC_FIND -- Get indices associated with labels.
%
%     Within a category, indices are calculated via an `or` operation.
%     Across categories, indices are calculated via an `and` operation.
%
%     E.g., if `loc` is a locator with labels `0` and `1` in category `0`,
%     then loc_find( loc, [0, 1] ) returns rows associated with `0` OR `1`.
%
%     But if `loc` is a locator with labels `0` and `1` in categories `0`
%     and `1`, respectively, then loc_find( [0, 1] ) returns the subset of
%     rows associated with `0` AND `1`.
%
%     See also loc_getlabs, loc_getcats
%
%     IN:
%       - `loc` (uint32) -- Locator id.
%       - `labels` (uint32) -- Labels
%     OUT:
%       - `indices` (uint32) -- Index of elements associated with `labels`.

op_code = loc_opcodes( 'find' );

indices = loc_api( op_code, loc, uint32(labels) );

end

