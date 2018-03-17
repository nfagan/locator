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

fprintf( '\nLocator:\n' );

tic;
initcat( loc, c('greetings'), makelabs(loc, greetings, l) );
initcat( loc, c('farewells'), makelabs(loc, farewells, l) );
toc;

tic;
[I, C] = findall( loc );

c_str = l(C)';

toc;

locator.destroyexcept( orig_locators );
multimap.destroyexcept( orig_maps );

%%  compare to categorical

fprintf( '\ncategorical:\n' );

tic;
categ = categorical( [greetings, farewells] );
toc;

tic;
[C2, ~, ib] = unique( categ, 'rows' );
n_occur = accumarray( ib, 1 );
I2 = accumarray( ib, (1:numel(ib))', [], @(rows) {sort(rows)} );
toc;
