function loc_test_resize()

loc_test_assert_depends_present();

sp = get_labels( get_example_container() );

loc = locator.from( sp );

labs_loc = getlabs( loc );

n_iters = 100;

for i = 1:n_iters
  z = copy( loc );
  
  new_size = randperm( numel(loc) * 2, 1 );
  
  resize( z, new_size );
  
  if ( new_size > numel(loc) )
    for j = 1:numel(labs_loc)
      assert( max(find(z, labs_loc(j))) <= numel(loc) );
    end
  elseif ( new_size < numel(loc) )
    labs_z = getlabs( z );
    for j = 1:numel(labs_z)
      assert( max(find(z, labs_z(j))) <= numel(loc) );
    end
  end
  
  destroy( z );
end


end