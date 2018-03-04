function instances = loc_instances()

%   LOC_INSTANCES -- Return ids of all active locators.
%
%     OUT:
%       - `instances` (uint32) -- Handle(s) to active locators.

op_code = loc_opcodes( 'instances' );

instances = loc_api( op_code );

end