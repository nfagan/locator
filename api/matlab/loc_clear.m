function loc = loc_clear(loc)

%   LOC_CLEAR -- Clear the locator of all contents.
%
%     See also loc_size, loc_empty, loc_isempty
%
%     IN:
%       - `loc` (uint32) -- Locator id.
%     OUT:
%       - `loc` (uint32) -- Locator id.

loc_keep( loc, [] );
loc_rmcat( loc, loc_getcats(loc) );

end

