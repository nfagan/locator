function loc_test_setcat_mult2()

loc_test_assert_depends_present();

sp = get_labels( get_example_container() );
repr = categorical( sp );

loc = locator();

n = size( repr, 1 );

for i = 1:size(repr, 2)
  requirecat( loc, i );
  setcat( loc, i, uint32(repr(:, i)), true(n, 1) );
end

end