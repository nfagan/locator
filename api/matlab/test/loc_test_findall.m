function loc_test_findall()

loc_test_assert_depends_present();

sp = get_labels( get_example_container() );

[loc, c, l] = locator.from( sp );

all_str_cats = keys( c );

% test all categories
test_some_cats( loc, c, l, sp, all_str_cats );

% test random subset of categories
for i = 1:10
  n = randi( numel(all_str_cats) - 1, 1, 1 );
  some_str_cats = all_str_cats( randperm(numel(all_str_cats), n) );
  
  fprintf( '\n Testing "%s".', strjoin(some_str_cats, ', ') );
  
  test_some_cats( loc, c, l, sp, some_str_cats );
end

fprintf( '\n' );

end

function test_some_cats(loc, c, l, sp, str_cats)

[I, C] = findall( loc, c(str_cats) );
[I2, C2] = rget_indices( sp, str_cats );

assert( numel(I) == numel(I2), 'Indices mismatch.' );
assert( size(C, 2) == size(C2, 1), 'Combinations mismatch.' );

matching_inds = false( 1, size(C, 2) );

for i = 1:size(C2, 1)
  numeric_labs = l(C2(i, :))';
  
  for j = 1:size(C, 2)
    c_subset = sort( C(:, j) );
    n_labs = sort( numeric_labs );
    matching_inds(j) = all( c_subset == n_labs );
  end
  
  assert( sum(matching_inds) == 1, 'Expected one row of indices to match.' );
  
  matching_i = I{matching_inds};
  matching_i2 = find( I2{i} );
  
  assert( isequal(matching_i, matching_i2), 'Indices differed.' );
end

end