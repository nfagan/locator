function loc_test_eq_several()

loc_test_assert_depends_present();

loc = locator.from( get_labels(get_example_container()) );

c_instances = locator.instances();

n_iters = 1e4;

try
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
  
  loc_test_destroy_new( c_instances );
  
catch err
  loc_test_destroy_new( c_instances );
  throw( err );
end

loc_test_util_print_ok( mfilename );

end