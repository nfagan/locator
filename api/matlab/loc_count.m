function counts = loc_count(loc, labels)

%   LOC_COUNT -- Get the number of rows associated with label(s).
%
%     See also loc_isempty, loc_empty, loc_clear
%
%     IN:
%       - `loc` (uint32) -- Locator id.
%       - `labels` (uint32) -- Labels to query.
%     OUT:
%       - `counts` (uint32) -- Number of rows in `loc`.

op_code = loc_opcodes( 'count' );

counts = loc_api( op_code, loc, uint32(labels) );

end