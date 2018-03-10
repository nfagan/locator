function loc_test_append()

loc_test_assert_depends_present();

sp = get_labels( get_example_container() );
[loc, ~, lab_map] = loc_from( sp );

n_iters = 100;

for i = 1:n_iters
  locb = loc_copy( loc );
  locc = loc_copy( loc );

  rand_subset_ind = sort( randperm(shape(sp, 1), 400) );
  rand_subset = sp.numeric_index( rand_subset_ind );

  loc_keep( locc, rand_subset_ind );

  sp_appended = append( sp, rand_subset );

  loc_append( locb, locc );

  assert( shape(sp_appended, 1) == loc_size(locb) );  

  for j = 1:numel(sp_appended.labels)
    char_lab = sp_appended.labels{j};
    sp_ind = find( sp_appended.where(char_lab) );
    loc_ind = loc_find( locb, lab_map(char_lab) );

    assert( numel(sp_ind) == numel(loc_ind) && all(sp_ind == loc_ind) );
  end

  loc_destroy( [locb, locc] );
end

end