function loc_test_eq_several()

loc_test_assert_depends_present();

loc = locator.from( get_labels(get_example_container()) );

n_iters = 1e4;

for i = 1:n_iters
  a = copy( loc );
  b = copy( loc );

  index = randperm( numel(a), 1 );

  keep(a, index);
  keep(b, index);

  assert( a == b );

  destroy( a );
  destroy( b );
end

end