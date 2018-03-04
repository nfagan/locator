loc_build( 'loc_profile_add_label.cpp', 'test' );

%%  

n_labs = 2e3;
sz = 1e1;
n_iters = 1e3;

%%  profile add label

tic();
for i = 1:n_iters
  loc_profile_add_label( n_labs, sz );
end
loc_time = toc();

fprintf( '\n %0.4f (s) (loc)', loc_time );

%%  profile add label, fastest

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
  
  
end

sp_time = toc();

fprintf( '\n %0.4f (s) (sp)', sp_time );

%%  profile add label, worst case

tic();

a_n = real( 'A' );

alph = char( a_n:a_n+n_labs-1 );

for i = 1:n_iters
  x = SparseLabels();
  
  x.indices = true( sz, 1 );
  
  x.labels = cell( n_labs, 1 );
  x.categories = cell( n_labs, 1 );
  
  x.labels{1} = char( a_n );
  x.categories{1} = char( a_n );
  
  for j = 2:n_labs
    label = char( a_n + j - 1 );
    x.labels{j} = label;
    x.categories{j} = label;
    x.indices(:, j) = true( sz, 1 );
  end
end

sp_time = toc();

fprintf( '\n %0.4f (s) (sp)', sp_time );
