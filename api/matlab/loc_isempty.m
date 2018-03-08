function tf = loc_isempty(loc)

%   LOC_ISEMPTY -- True if the locator has no labels, and thus a size of 0.
%
%     See also loc_size, loc_getlabs
%
%     IN:
%       - `loc` (uint32) -- Locator id.
%     OUT:
%       - `tf` (logical)

tf = loc_size( loc ) == 0;

end