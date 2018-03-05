loc_destroy();

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

%%

loc_rmcat( loc, category );