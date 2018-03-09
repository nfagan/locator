function loc_test_eq()

loc_test_assert_depends_present();

sp = get_labels( get_example_container() );

a = loc_from( sp );
b = loc_from( sp );
c = loc_from( sp );

instances = [ a, b, c ];

try
  labs_a = loc_getlabs( a );

  lab = labs_a(1);

  ind = loc_find( a, lab );

  cat = loc_whichcat( a, lab );

  loc_setcat( a, cat, lab, ind(1:end-1), loc_size(a) );
  loc_setcat( c, cat, lab, ind, loc_size(c) );

  assert( ~loc_eq(a, b), 'locs were equal' );
  assert( loc_eq(b, c), 'locs were not equal' );
  
catch err
  loc_destroy( instances );
  throw( err );
end

loc_destroy( instances );

loc_test_util_print_ok( mfilename );

end