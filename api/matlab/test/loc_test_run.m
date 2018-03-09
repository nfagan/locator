function loc_test_run(func)

c_instances = locator.instances();

func_name = func2str( func );

try
  func();
  loc_test_util_print_ok( func_name );
catch err
  fprintf( '\n `%s` failed with message:\n %s\n', func2str(func), err.message );
end

loc_test_destroy_new( c_instances );
  

end