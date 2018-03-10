function loc_test_append_random()

loc_test_assert_depends_present();

sp = get_labels( get_example_container() );

[y, ~, lab_map_z] = locator.from( sp );
lab_map_values = cell2mat( lab_map_z.values() );
lab_map_keys = lab_map_z.keys();

n_iters = 1e3;

indices = randperm( numel(y), n_iters );

z = requirecat( locator(), getcats(y) );

for i = 1:n_iters
  x = keep( copy(y), indices(i) );

  labs = getlabs( x );

  for j = 1:numel(labs)
    assert( find(x, labs(j)) == 1 );
  end

  x2 = requirecat( locator(), getcats(x) );

  for j = 1:numel(labs)
    setcat( x2, whichcat(x, labs(j)), labs(j), true );
    assert( whichcat(x2, labs(j)) == whichcat(x2, labs(j)) );
  end
  
  append( z, x2 );

  destroy( x );
end

sp2 = SparseLabels();

for i = 1:n_iters
  sp2 = append( sp2, numeric_index(sp, indices(i)) );
end

[z2, ~, lab_map_z2] = locator.from( sp2 );

z2_labs = getlabs( z2 );
z_labs = getlabs( z );

assert( numel(z2_labs) == numel(z_labs) );

for i = 1:numel(z_labs)
  z_lab = z_labs(i);
  z_lab_str = lab_map_keys{lab_map_values == z_lab};
  matching_z2_lab = lab_map_z2( z_lab_str );
  
  assert( isequal(find(z2, matching_z2_lab), find(z, z_lab)) );  
end

end