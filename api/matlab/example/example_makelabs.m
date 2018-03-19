function example_makelabs()

%%  make labs

orig_locators = locator.instances();
orig_maps = multimap.instances();

l = multimap();
c = multimap();

loc = locator();

c('greetings') = randlab( loc );
c('farewells') = randlab( loc );

sz = 1e5;

his =     repmat( {'hi'}, sz, 1 );
hellos =  repmat( {'hello'}, sz, 1 );
sups =    repmat( {'sup'}, sz, 1 );
byes =    repmat( {'bye'}, sz, 1 );
adios =   repmat( {'adios'}, sz, 1 );
peaces =  repmat( {'peace'}, sz, 1 );

greetings = [ his; hellos; sups ];
greetings = greetings( randperm(sz * 3) );

farewells = [ byes; adios; peaces ];
farewells = farewells( randperm(sz * 3) );

profile_str = '(create)';
fprintf( '\nlocator: %s\n', profile_str );

tic;
initcat( loc, c('greetings'), makelabs(loc, greetings, l) );
initcat( loc, c('farewells'), makelabs(loc, farewells, l) );
toc;

profile_str = '(find all)';
fprintf( '\nlocator: %s\n', profile_str );

tic;
[I, C] = findall( loc );

c_str = l(C)';

toc;

%%  compare to categorical

profile_str = '(create)';

fprintf( '\ncategorical: %s\n', profile_str );

tic;
categ = categorical( [greetings, farewells] );
toc;

profile_str = '(find all)';

fprintf( '\ncategorical: %s\n', profile_str );

tic;
[C2, ~, ib] = unique( categ, 'rows' );
n_occur = accumarray( ib, 1 );
I2 = accumarray( ib, (1:numel(ib))', [], @(rows) {sort(rows)} );
toc;

%%  compare find, one category

profile_str = '(find, one category)';

fprintf( '\nlocator: %s\n', profile_str );

tic;
for i = 1:1e3
  ind = find( loc, get(l, {'peace', 'bye', 'adios'}) );
end
toc;

fprintf( '\ncategorical: %s\n', profile_str );
tic;
categ_farewells = categ(:, 2);
for i = 1:1e3
  ind2 = categ_farewells == 'peace' | categ_farewells == 'bye' | categ_farewells == 'adios';
end
toc;

assert( isequal(ind, find(ind2)) );

%%  compare find, multiple categories

profile_str = '(find, multiple categories)';

fprintf( '\nlocator: %s\n', profile_str );

tic;
for i = 1:1e3
  ind = find( loc, get(l, {'peace', 'bye', 'hello'}) );
end
toc;

fprintf( '\ncategorical: %s\n', profile_str );
tic;
categ_greetings = categ(:, 1);
categ_farewells = categ(:, 2);
for i = 1:1e3
  ind2 = categ_greetings == 'hello' & (categ_farewells == 'bye' | categ_farewells == 'peace');
end
toc;

assert( isequal(ind, find(ind2)) );

%%  destroy

locator.destroyexcept( orig_locators );
multimap.destroyexcept( orig_maps );

