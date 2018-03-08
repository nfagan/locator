index_sz = 1e6;

n_true = index_sz / 1e2;

n_categories = 7;
n_labels = 1e3;

alphabet = ['a':'z', 'A':'Z'];
n_alph = numel( alphabet );
n_alph_perm = 20;

cat_names = arrayfun( @(x) alphabet(randi(n_alph, 1, n_alph_perm)), 1:n_categories, 'un', false );
cat_names = unique( cat_names );

n_categories = numel( cat_names );

index = false( index_sz, 1 );

for i = 1:n_labels
  
  index(:) = false;
  
  place_true = unique( randi(index_sz, n_true, 1) );
  
  index(place_true) = true;
  
  label = alphabet(randi(n_alph, 1, n_alph_perm));
  
  while ( sp.contains(label) )
    label = alphabet(randi(n_alph, 1, n_alph_perm));
  end
  
  in_cat = randi( n_categories, 1, 1 );
  c_cat = cat_names{in_cat};
  
  if ( i == 1 )
    sp = SparseLabels( {struct('label', label, 'category', c_cat, 'index', index)} );
    continue;
  end
  
  if ( ~sp.contains_categories(c_cat) )
    sp = sp.add_category( c_cat );
  end
  
  sp = sp.set_category( c_cat, label, index );
  
end

%%

labs = sp.labels;

n_search = 50;
n_iters = 1e2;

cont = Container( zeros(size(index)), sp );

t = zeros( 1, n_iters );

for i = 1:n_iters  
  search_labs = labs( randperm(numel(labs), n_search) );
  
  t0 = tic();
  
  ind = cont.where( search_labs );
  
  t(i) = toc( t0 ) * 1e3;
end

fprintf( '\n Mean (where): %0.3f (ms)', mean(t) );
fprintf( '\n Total (where): %0.3f (ms)', sum(t) );

%%

tic; 
x = cont.get_indices( cat_names(1:2) );
toc;


