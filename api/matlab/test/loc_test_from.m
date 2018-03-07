function loc_test_from()

if ( isempty(which('get_example_container')) )
  error( ['This function depends on the `global` repository, available' ...
    , ' at: https://github.com/nfagan/global. Download / clone the repository' ...
    , ' and add it to matlab''s search path.'] );
end

cont = get_example_container();

sp = cont.labels;

[loc, cats, labs] = loc_from( sp );

all_labs = sp.labels;

for i = 1:numel(all_labs)
  sp_ind = sp.where( all_labs{i} );
  loc_ind = loc_find( loc, labs(all_labs{i}) );
  
  assert( all(find(sp_ind) == loc_ind) );
end

[I, C] = sp.rget_indices( {'day', 'dose', 'image', 'session'} );

loc_inds = arrayfun( @(x) labs(x{1}), C );

for i = 1:numel(I)
  
  sp_ind = sp.where( C(i, :) );
  
  loc_ind = loc_find( loc, loc_inds(i, :) );
  
  assert( all(find(sp_ind) == loc_ind) );
end

loc_destroy( loc );

end