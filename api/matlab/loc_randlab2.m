function lab = loc_randlab2(loc_a, loc_b)

%   LOC_RANDLAB2 -- Get a label that does not exist between two locators.
%
%     IN:
%       - `loc_a` (uint32) -- Locator id 1.
%       - `loc_b` (uint32) -- Locator id 2.
%     OUT:
%       - `lab` (uint32)

lab = loc_api( loc_opcodes('get_rand_lab2'), uint32(loc_a), uint32(loc_b) );

end