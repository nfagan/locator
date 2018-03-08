function loc_test_pipeline()

loc = loc_create();

category = 100;
label = 0;
sz = 1e4;
categories = category:category+200;

loc_requirecat( loc, categories );

loc_setcat( loc, category, label, true(sz, 1) );

assert( loc_size(loc) == sz );

assert( loc_hascat(loc, category) );

assert( all(loc_find(loc, label)' == 1:sz) );

lab = loc_getlabs( loc );

assert( numel(lab) == 1 && lab == label );

loc_rmcat( loc, category );

assert( loc_size(loc) == 0 );
assert( numel(loc_getlabs(loc)) == 0 );
assert( numel(loc_getcats(loc)) == numel(categories) - 1 );
assert( ~loc_hascat(loc, category) );

loc_requirecat( loc, categories );

assert( numel(loc_getcats(loc)) == numel(categories) );

for i = 1:10
  loc_setcat( loc, category + i - 1, i, true(sz, 1) );
end

for i = 1:10
  assert( loc_hascat(loc, category + i - 1) );
end

loc_destroy( loc );

% second

loc = loc_create();

loc_requirecat( loc, categories );

for i = 1:numel(categories)
  loc_setcat( loc, categories(i), i, true(sz, 1) );
end

assert( numel(loc_getlabs(loc)) == numel(categories) );
assert( numel(loc_getcats(loc)) == numel(categories) );

assert( loc_size(loc) == sz );

loc_rmcat( loc, categories );

assert( numel(loc_getcats(loc)) == 0 );

loc_destroy( loc );

fprintf( '\n OK: `loc_test_pipeline` passed.\n' );

end