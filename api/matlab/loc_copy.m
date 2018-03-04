function locb = loc_copy(loc)

%   LOC_COPY -- Copy an existing locator.
%
%     IN:
%       - `loc` (uint32) -- Locator id.
%     OUT:
%       - `locb` (uint32) -- Id of the copied locator.

op_code = loc_opcodes( 'copy' );

locb = loc_api( op_code, loc );

end

