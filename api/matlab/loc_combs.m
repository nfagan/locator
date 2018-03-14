function cmbs = loc_combs(loc, categories)

%   LOC_COMBS -- Get present combinations of labels in categories.
%
%     See also loc_getlabs, loc_getcats, loc_find
%
%     IN:
%       - `loc` (uint32) -- Locator id.
%       - `categories` (uint32) -- Categories from which to draw labels.
%     OUT:
%       - `cmbs` (uint32) -- Present combinations.

if ( numel(categories) == 1 )
  cmbs = loc_incat( loc, categories )';
  return;
end

op_code = loc_opcodes( 'combinations' );

cmbs = loc_api( op_code, loc, uint32(categories) );

if ( isempty(cmbs) )
  return;
end

n_cats = numel( categories );

cmbs = reshape( cmbs, n_cats, numel(cmbs) / n_cats );

end

