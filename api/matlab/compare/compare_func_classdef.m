%%  add cat

tic;
z = loc_create();
for i = 1:1e4
  loc_addcat( z, i );
end
toc;
loc_destroy( z );

%%  add cat, locator

tic;
z = locator();
for i = 1:1e4
  z.addcat(i);
end
toc;

%%  setcat

tic;
z = loc_create();
loc_addcat( z, 0 );
for i = 1:1e4
  loc_setcat( z, 0, i, true(100, 1) );
end
toc;

%%  setcat

tic;
z = locator();
z.addcat( 0 );
for i = 1:1e4
  z.setcat( 0, i, true(100, 1) );
end
toc;

%%  find

z = loc_from( get_labels(get_example_container()) );

tic;
for i = 1:1e3
  loc_find( z, 1:200 );
end
toc;

%%

z = locator.from( get_labels(get_example_container()) );

tic;
for i = 1:1e3
  z.find( 1:200 );
end
toc;

%%

z = locator.from( get_labels(get_example_container()) );

tic;
for i = 1:1e4
  s = z.size();
end
toc;

%%

z = loc_from( get_labels(get_example_container()) );

tic;
for i = 1:1e4
  s = loc_size( z );
end
toc;



