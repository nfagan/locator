function loc_test_keepeach()

loc_test_assert_depends_present();

cont = get_example_container();

labs = labeler.from( cont.labels );

for idx = 1:10
  
  labs_copy = copy( labs );

  cats = getcats( labs_copy );
  
  n_cats = randi( numel(cats), 1, 1 );
  rand_inds = randperm( numel(cats), n_cats );
  
  subset_cats = cats(rand_inds);
  
  keepeach( labs_copy, subset_cats );

  full_cat = false( size(labs_copy, 1), 1 );

  for i = 1:numel(cats)
    in_cat = incat( labs_copy, cats(i) );

    for j = 1:numel(in_cat)
      ind = find( labs_copy, in_cat(j) );
      assert( ~any(full_cat(ind)) );
      full_cat(ind) = true;
    end

    assert( all(full_cat) );

    full_cat(:) = false;
  end
  
  destroy( labs_copy );

end

all_cats = strcats( labs );

for i = 1:10
  
  n_cats = randi( numel(all_cats), 1, 1 );
  rand_inds = randperm( numel(all_cats), n_cats );
  
  subset_cats = all_cats(rand_inds);
  
  [y, I, C] = keepeach( copy(labs), subset_cats );
  
  for j = 1:numel(I)
    subset_indexed = one( keep(copy(labs), I{j}) );
    subset_eached = keep( copy(y), j );
    
    assert( subset_indexed == subset_eached, 'Subsets didn''t match.' );
    
    subset_indexed_labs = sort( strlabs(subset_indexed) );
    subset_eached_labs = sort( strlabs(subset_eached) );
    
    assert( isequal(subset_indexed_labs, subset_eached_labs), 'Labels didn''t match.' );
    
    destroy( subset_indexed );
    destroy( subset_eached );
  end
end

end

function check_equal_labs(labs, sp)

unq_labs = strlabs( labs );
unq_sp = sp.labels;

assert( numel(unq_labs) == numel(unq_sp), 'Number of labels didn''t match.' );

for i = 1:numel(unq_labs)
  lab = unq_labs{i};
  
  left_carrot_ind = strfind( lab, '<' );
  
  if ( ~isempty(left_carrot_ind) )
    right_carrot_ind = strfind( lab, '>' );
    search_lab = sprintf( 'all__%s', lab(left_carrot_ind+1:right_carrot_ind-1) );
  else
    search_lab = lab;
  end
  
  assert( sum(strcmp(unq_sp, search_lab)) == 1, 'Labels didn''t match.' );
end

end