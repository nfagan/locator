function loc_destroy(loc)

%   LOC_DESTROY -- Delete locator instances.
%
%     loc_destroy( loc ) destroys locator(s) associated with id(s) `loc`.
%     loc_destroy() destroys all locators.
%
%     See also loc_create, loc_instances
%
%     IN:
%       - `loc` (uint32) |OPTIONAL| -- Id(s) of the locator(s) to destroy.

op_code = loc_opcodes( 'destroy' );

if ( nargin < 1 || isempty(loc) )
  loc_api( op_code );
else
  arrayfun( @(x) loc_api(op_code, x), uint32(loc) );
end

end