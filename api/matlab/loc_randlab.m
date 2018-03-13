function lab = loc_randlab(loc)

%   LOC_RANDLAB -- Get a label that does not exist in the locator.
%
%     IN:
%       - `loc` (uint32) -- Locator id.
%     OUT:
%       - `lab` (uint32)

lab = loc_api( loc_opcodes('get_rand_lab'), uint32(loc) );

end