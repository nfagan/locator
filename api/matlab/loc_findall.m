function [I, C] = loc_findall(loc, categories)

%   LOC_COMBS -- Get indices of combinations of labels in categories.
%
%     See also loc_combs, loc_getlabs, loc_getcats, loc_find
%
%     IN:
%       - `loc` (uint32) -- Locator id.
%       - `categories` (uint32) -- Categories from which to draw labels.
%     OUT:
%       - `I` (uint32) -- indices.
%       - `C` (uint32) -- combinations.

if ( numel(categories) == 1 )
  C = loc_incat( loc, categories )';
  I = arrayfun( @(x) loc_find(loc, x), C, 'un', false );
  return;
end

op_code = loc_opcodes( 'find_all' );

[I, C] = loc_api( op_code, loc, uint32(categories) );

if ( isempty(C) )
  return;
end

n_cats = numel( categories );

C = reshape( C, n_cats, numel(C) / n_cats );

end

