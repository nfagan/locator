function tf = loc_isloc(loc)

%   LOC_ISLOC -- Return whether input(s) are valid locators.
%
%     IN:
%       - `loc` (uint32) -- Locator id.
%     OUT:
%       - `tf` (logical)

tf = false( size(loc) );

if ( ~isa(loc, 'uint32') )
  return;
end

op_code = loc_opcodes( 'is_loc' );

tf = arrayfun( @(x) loc_api(op_code, x), loc );

end