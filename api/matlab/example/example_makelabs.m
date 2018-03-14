%%  make labs

orig_locators = locator.instances();
orig_maps = multimap.instances();

l = multimap();
c = multimap();

loc = locator();

c('greetings') = randlab( loc );
c('farewells') = randlab( loc );

sz = 1e4;

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

initcat( loc, c('greetings'), makelabs(loc, greetings, l) );
initcat( loc, c('farewells'), makelabs(loc, farewells, l) );

tic;
[I, C] = findall( loc );
toc;

c_str = l(C);
c_str = c_str';

locator.destroyexcept( orig_locators );
multimap.destroyexcept( orig_maps );