function loc_destroy(loc)

%   LOC_DESTROY -- Destroy a locator, or all locators.
%
%     IN:
%       - `loc` (uint32) |OPTIONAL| -- Id of the locator to destroy.

op_code = loc_opcodes( 'destroy' );

if ( nargin < 1 || isempty(loc) )
  loc_api( op_code );
else
  loc_api( op_code, uint32(loc) );
end

end