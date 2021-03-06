function loc_test_from()

loc_test_assert_depends_present()

cont = get_example_container();

sp = cont.labels;

[loc, cat_map, lab_map] = loc_from( sp );

all_labs = sp.labels;

for i = 1:numel(all_labs)
  sp_ind = sp.where( all_labs{i} );
  loc_ind = loc_find( loc, lab_map(all_labs{i}) );

  assert( isequal(find(sp_ind), loc_ind), 'Label indices were not equal.' );
end

[I, C] = sp.rget_indices( {'day', 'dose', 'image', 'session'} );

loc_inds = arrayfun( @(x) lab_map(x{1}), C );

for i = 1:numel(I)

  sp_ind = sp.where( C(i, :) );

  loc_ind = loc_find( loc, loc_inds(i, :) );

  assert( isequal(find(sp_ind), loc_ind) );
end

end