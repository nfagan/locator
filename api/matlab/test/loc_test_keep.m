function loc_test_keep()

loc_test_assert_depends_present();

cont = get_example_container();

sp = cont.labels;

[loc, ~, lab_map] = loc_from( sp );

sz = loc_size( loc );

n_keep = 100;

assert( n_keep <= sz );

for i = 1:1e3
  locb = loc_copy( loc );
  
  to_keep = randperm( sz, n_keep );
  
  loc_keep( locb, to_keep );
  
  assert( loc_size(locb) == n_keep );
  
  sp_ind = false( shape(sp, 1), 1 );
  sp_ind(to_keep) = true;
  
  sp_kept = sp.keep( sp_ind );
  
  labs_kept = sp_kept.labels;
  lab_inds_kept = cellfun( @(x) lab_map(x), labs_kept );
  
  loc_labs = loc_getlabs( locb );
  
  assert( numel(loc_labs) == numel(lab_inds_kept) );  
  
  assert( all(sort(lab_inds_kept) == loc_labs) );
  
  loc_destroy( locb );
end

loc_destroy( loc );

end