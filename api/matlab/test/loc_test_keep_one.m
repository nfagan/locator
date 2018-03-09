function loc_test_keep_one()

loc_test_assert_depends_present();

sp = get_labels( get_example_container() );

c_instances = locator.instances();

try

  [loc, cat_map, lab_map] = locator.from( sp );

  lab_keys = lab_map.keys();
  lab_values = cell2mat( lab_map.values() );

  n_iters = 1e4;

  for i = 1:n_iters
    ind = randperm( numel(loc), 1 );

    loc_b = copy( loc );

    keep( loc_b, ind );
    sp_b = numeric_index( sp, ind );

    loc_labs = getlabs( loc_b );

    assert( numel(sp_b.labels) == numel(loc_labs) );
    assert( all(arrayfun(@(x) find(loc_b, x) == 1, loc_labs)) );

    num_inds = arrayfun( @(x) find(lab_values == x), loc_labs );
    loc_lab_str = lab_keys( num_inds );

    assert( isequal(sort(sp_b.labels(:)), sort(loc_lab_str(:))) );  

    destroy( loc_b );
  end
  cleanup( c_instances );
catch err
  
  cleanup( c_instances );
  throw( err );
end

end

function cleanup(old_instances)
all_instances = locator.instances();
loc_destroy( setdiff(all_instances, old_instances) );
end