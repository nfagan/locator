function cats = loc_fullcat(loc, categories)

%   LOC_FULLCAT -- Get the full category(ies) in the locator.
%
%     See also loc_hascat, loc_find
%
%     IN:
%       - `loc` (uint32) -- Locator id.
%       - `categories` (uint32)
%     OUT:
%       - `cats` (uint32) -- Full categories.

op_code = loc_opcodes( 'full_cat' );

cats = loc_api( op_code, loc, uint32(categories) );

sz = loc_size( loc );

if ( numel(categories) > 1 && sz > 0 )
  cats = reshape( cats, sz, numel(categories) );
end

end

