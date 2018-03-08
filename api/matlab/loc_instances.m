function instances = loc_instances(index)

%   LOC_INSTANCES -- Return ids of all active locators.
%
%     instances = loc_instances(); returns the ids of all active locators.
%     instance = loc_instances(1); returns the first id, if it exists.
%
%     See also loc_create, loc_destroy
%
%     IN:
%       - `index` (uint32) |OPTIONAL| -- Subset of locator ids to return.
%     OUT:
%       - `instances` (uint32) -- Handle(s) to active locators.

op_code = loc_opcodes( 'instances' );

instances = loc_api( op_code );

if ( nargin == 0 )
  return;
end

instances = instances( index );

end