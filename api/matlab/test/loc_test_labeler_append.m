function loc_test_labeler_append()

loc_test_assert_depends_present();

sp = get_labels( get_example_container() );

sp_rebuilt = SparseLabels();

N = 1e3;

for i = 1:N
  sp_rebuilt = append( sp_rebuilt, numeric_index(sp, i) );
end

L = labeler();

for i = 1:N
  l = labeler.from( numeric_index(sp, i) );
  L = append( L, l );
  destroy( l );
end

curr_labs = sort( strlabs(L) );
curr_labs = curr_labs(:);

assert( isequal(curr_labs, sort(sp_rebuilt.labels(:))) );

for i = 1:numel(curr_labs)
  ind_lab = find( L, curr_labs{i} );
  ind_sp = where( sp_rebuilt, curr_labs{i} );
  
  assert( isequal(find(ind_sp), ind_lab) );
end

end