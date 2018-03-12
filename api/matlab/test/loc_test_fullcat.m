function loc_test_fullcat()

loc_test_assert_depends_present();

sp = get_labels( get_example_container() );

loc = locator.from( sp );

cats = loc.getcats();

z = loc.fullcat( cats );

for i = 1:size(z, 2)
  unqs = unique( z(:, i) );
  
  assert( isequal(unqs, loc.incat(cats(i))) );
  
  for j = 1:numel(unqs)
    assert( isequal(find(z(:, i) == unqs(j)), find(loc, unqs(j))) );
  end
end

end