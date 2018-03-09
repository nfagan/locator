function loc_test_append_random()

loc_test_assert_depends_present();

sp = get_labels( get_example_container() );
sp = sort_labels( sp );

y = locator.from( sp );

n_iters = 1e3;

% indices = randperm( numel(y), n_iters );
indices = ones( 1, n_iters );

z = requirecat( locator(), getcats(y) );

try

  for i = 1:n_iters
    x = keep( copy(y), indices(i) );

    labs = getlabs( x );

    for j = 1:numel(labs)
      assert( find(x, labs(j)) == 1 );
    end

    x2 = requirecat( locator(), getcats(x) );

    for j = 1:numel(labs)
      setcat( x2, whichcat(x, labs(j)), labs(j), true );
    end

  %   append( z, x );
    append( z, x2 );

    destroy( x );
  end

  sp2 = SparseLabels();

  for i = 1:n_iters
    sp2 = append( sp2, numeric_index(sp, indices(i)) );
  end

  sp2 = sort_labels( sp2 );

  z2 = locator.from( sp2 );

  labs = getlabs( z2 );
  
  assert( size(z2, 1) == size(z, 1), 'labs were not equal size' );
  
  for i = 1:numel(labs)
    
    assert( haslab(z, labs(i)), 'label was missing in z' );
    assert( haslab(z2, labs(i)), 'label was missing in z2' );
    
    assert( isequal(find(z, labs(i)), find(z2, labs(i))), 'labs were not equal' );
  end
  
  cleanup( z2, z, y );
catch err
  cleanup( z2, z, y );
  throw( err );
end

loc_test_util_print_ok( mfilename );

end

function cleanup( varargin )
cellfun( @destroy, varargin );
end