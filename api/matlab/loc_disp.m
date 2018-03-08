function loc_disp(loc)

%   LOC_DISP -- Display info about the locator.
%
%     IN:
%       - `loc` (uint32) -- Locator id.

sz = loc_size( loc );
cats = loc_getcats( loc );
labs = loc_getlabs( loc );

sz_str = sprintf( '%d×1', sz );

n_cats = numel( cats );
n_labs = numel( labs );

if ( n_cats == 1 )
  cat_str = 'category';
else
  cat_str = 'categories';
end

if ( n_labs == 1 )
  lab_str = 'label';
else
  lab_str = 'labels';
end

fprintf( '\n %s locator', sz_str );
fprintf( '\n %d %s, %d %s', n_cats, cat_str, n_labs, lab_str );
fprintf( '\n\n' );

end