function loc_test_from()

loc_test_assert_depends_present()

cont = get_example_container();

sp = cont.labels;

[loc, cat_map, lab_map] = loc_from( sp );

try
  all_labs = sp.labels;

  for i = 1:numel(all_labs)
    sp_ind = sp.where( all_labs{i} );
    loc_ind = loc_find( loc, lab_map(all_labs{i}) );

    assert( all(find(sp_ind) == loc_ind) );
  end

  [I, C] = sp.rget_indices( {'day', 'dose', 'image', 'session'} );

  loc_inds = arrayfun( @(x) lab_map(x{1}), C );

  for i = 1:numel(I)

    sp_ind = sp.where( C(i, :) );

    loc_ind = loc_find( loc, loc_inds(i, :) );

    assert( all(find(sp_ind) == loc_ind) );
  end

catch err
  loc_destroy( loc );
  throw( err );
end

loc_destroy( loc );

fprintf( '\n OK: `loc_test_from` passed.\n' );

end