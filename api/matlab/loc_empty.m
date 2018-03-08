function loc = loc_empty(loc)

%   LOC_EMPTY -- Make the locator of size 0, but retain categories.
%
%     See also loc_isempty, loc_size, loc_clear
%
%     IN:
%       - `loc` (uint32) -- Locator id.
%     OUT:
%       - `loc` (uint32) -- Locator id.

loc_keep( loc, [] );

end

