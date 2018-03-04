loc_build( 'loc_profile_find.cpp', 'test' );

%%  

n_labs = 100;
n_search = min( 4, n_labs );
sz = 1e5;
n_iters = 1e3;

%%  profile find

tic();
for i = 1:1e3
  ind = loc_profile_find( n_labs, n_search, sz );
end
loc_time = toc();

fprintf( '\n %0.3f (s) (loc)', loc_time );

%%  profile find

tic();

a_n = real( 'A' );

for i = 1:n_iters
  x = SparseLabels();
  
  x.indices = true( sz, n_labs );
  x.labels = cell( n_labs, 1 );
  x.categories = cell( n_labs, 1 );
  
  for j = 1:n_labs
    label = char( a_n + j - 1 );
    x.labels{j} = label;
    x.categories{j} = label;
  end
  
  ind = where( x, x.labels(1:n_search) );
  
end

sp_time = toc();

fprintf( '\n %0.3f (s) (sp)', sp_time );