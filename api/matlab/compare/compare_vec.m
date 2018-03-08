tic;

x = 1:1e5;

y = toc;

disp( y * 1e3 );

%%

y = zeros( 100, 1 );

n_iters = 1;

z = cell(1, n_iters);

for i = 1:n_iters

  tic;

  z{i} = int8( 1:100000 );
  
  if ( i > 1 )
    z{i-1}(2:1000) = i;
  end

  y(i) = toc;
  
end

disp( sum(y) * 1e3 );

%%

A = false( 2e3, 10 );
B = false( 2e3, 10 );

tic();

for i = 1:100
  A = [ A; B ];
end

y = toc();

fprintf( '\n %0.3f (ms)', y * 1e3 );

%%  test element wise and / or

n_iters = 1000;
sz = 1e6;

a = zeros( n_iters, 1 );

for i = 1:n_iters
  
tic;

A = false( sz, 1 );
B = false( sz, 1);

% A(100) = true;

z = A | B;

a(i) = toc * 1e3;

end

fprintf( '\n Mean time: %0.4f (ms)', mean(a) );

%%  test all

x = true( 1000000, 1 );

tic();
y = all( x );
z = toc();

fprintf( '\n Time for all: %0.4f (ms)', z*1000 );

%%

tic;
y = true(1e8, 1);
z = [y; y(1:end-1)];
toc();

%%  test keep

n_iters = 1e3;

a = zeros( n_iters, 1 );

for i = 1:n_iters
  
tic();

y = false( 1e5, 1 );
y(10) = true;
y(12) = true;
y(14) = true;

inds = [10, 12, 14, 15];

x = y(inds);

a(i) = toc() * 1000;

end

fprintf( '\n Mean: %0.4f (ms)', mean(a) );

%%  test append

a1 = tic();

n_iters = 1e3;

sz = 1e7;

a = zeros( n_iters, 1 );

for i = 1:n_iters

a2 = tic();

x = false( sz, 1 );
% y = false( 5e5, 1 );
y = x;
y(end-10) = true;
z = [ x; y ];

a(i) = toc( a2 ) * 1e3;

end

z2 = toc( a1 );

fprintf( '\n Mean: %0.4f (ms)', mean(a) );
fprintf( '\n Total: %0.4f (ms)', sum(a) );

%%  test unaligned append

a1 = tic();

n_iters = 1e3;

sz = 1e5 + 11;

a = zeros( n_iters, 1 );

for i = 1:n_iters
  
  % x = false( 32, 1 );
x = false( 1, 1 );
y = false( sz, 1 );

y(end-1e3:end) = true;

a2 = tic();

z = [ x; y ];
a(i) = toc( a2 ) * 1e3;

end

z2 = toc( a1 );

fprintf( '\n Mean: %0.4f (ms)', mean(a) );
fprintf( '\n Total: %0.4f (ms)', sum(a) );

%%  test find

a1 = tic();

n_iters = 1e3;

sz = 1e5 + 2032;

a = zeros( n_iters, 1 );

for i = 1:n_iters
  
  % x = false( 32, 1 );
x = false( 1, 1 );
y = false( sz, 1 );

inds = randperm( sz, 1e2 );

y(inds) = true;

a2 = tic();

z = find( y );

a(i) = toc( a2 ) * 1e3;

end

z2 = toc( a1 );

fprintf( '\n Mean: %0.4f (ms)', mean(a) );
fprintf( '\n Total: %0.4f (ms)', sum(a) );

%%  test sum

a1 = tic();

n_iters = 1e3;

sz = 1e5;

a = zeros( n_iters, 1 );

for i = 1:n_iters
  
x = false( sz, 1 );

% inds = randperm( sz, 1e2 );
inds = 1:100;

x(inds) = true;

a2 = tic();

z = sum( x );

a(i) = toc( a2 ) * 1e3;

end

z2 = toc( a1 );

fprintf( '\n Mean: %0.4f (ms)', mean(a) );
fprintf( '\n Total: %0.4f (ms)', sum(a) );



%%

x = get_example_container();
z = Container.concat( repmat({x}, 1, 1) );

tic;
[i,c] = z.get_indices( z.categories() );
toc;

tic;
[i, c] = x.get_indices( x.categories() );
toc;






