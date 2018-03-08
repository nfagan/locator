function labs = loc_getlabs(loc)

%   LOC_GETLABS -- Get the label(s) in the locator.
%
%     See also loc_getcats, loc_whichcat
%
%     IN:
%       - `loc` (uint32) -- Locator id.
%     OUT:
%       - `labs` (uint32) -- Labels.

op_code = loc_opcodes( 'get_labs' );

labs = loc_api( op_code, loc );

end

