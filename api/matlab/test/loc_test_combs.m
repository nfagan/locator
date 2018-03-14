function loc_test_combs()

loc_test_assert_depends_present();

sp = get_labels( get_example_container() );

[loc, cats, labs] = locator.from( sp );

lab_keys = keys( labs );
lab_vals = values( labs );

cats_l = getcats( loc );

C = combs( loc, cats_l );

for i = 1:size(C, 2)
  subset_c = C(:, i);
  
  lab_inds = arrayfun( @(x) find(lab_vals == x), subset_c );
  lab_matches = lab_keys( lab_inds );
  
  ind_sp = find( sp.where(lab_matches) );
  ind_loc = find( loc, subset_c );
  
  assert( numel(ind_sp) == numel(ind_loc) && all(ind_sp == ind_loc) );
end

end
