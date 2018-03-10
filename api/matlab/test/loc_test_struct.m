function loc_test_struct()

loc_test_assert_depends_present()

cont = get_example_container();

sp = cont.labels;

[loc, cat_map, lab_map] = loc_from( sp );

s = loc_struct( loc );

lab_map_keys = lab_map.keys();
lab_map_values = cell2mat( lab_map.values() );
cat_map_keys = cat_map.keys();
cat_map_values = cell2mat( cat_map.values() );

for i = 1:numel(s.labels)
  lab = s.labels(i);
  in_cat = s.categories(i);

  ind_lab_in_keys = find( lab_map_values == lab );
  lab_key = lab_map_keys{ind_lab_in_keys};

  ind_cat_in_keys = find( cat_map_values == in_cat );
  cat_key = cat_map_keys{ind_cat_in_keys};

  ind_lab_in_sp = find( strcmp(sp.labels, lab_key) );

  assert( strcmp(sp.categories{ind_lab_in_sp}, cat_key) );

  ind_loc = s.indices(:, i);
  ind_sp = sp.where( lab_key );

  assert( all(ind_loc == ind_sp) );  
end

end