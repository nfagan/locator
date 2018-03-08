function tf = loc_haslab(loc, cats)

%   LOC_HASLAB -- Return whether the locator has the given label(s).
%
%     See also loc_hascat, loc_find
%
%     IN:
%       - `loc` (uint32) -- Id.
%       - `labs` (uint32) -- Labels.
%     OUT:
%       - `tf` (logical)

op_code = loc_opcodes( 'has_lab' );

tf = loc_api( op_code, loc, uint32(cats) );

end