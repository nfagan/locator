function n = loc_nlabs(loc)

%   LOC_NLABS -- Get the number of labels in the locator.
%
%     See also loc_count, loc_empty, loc_clear
%
%     IN:
%       - `loc` (uint32) -- Locator id.
%     OUT:
%       - `n` (uint32)

op_code = loc_opcodes( 'n_labels' );

n = loc_api( op_code, loc );

end