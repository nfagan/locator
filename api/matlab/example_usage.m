clear all;

loc_build( 'loc_api.cpp' );
loc_build( 'loc_opcodes.cpp' );

%%  create

tic;
loc = loc_create();
toc;

assert( loc_isloc(loc) );

cat = uint32(100);

%%  destroy

loc_destroy();

%%  add category

tic;

all_cats = uint32(cat:cat+1e4);

loc_requirecat( loc, all_cats );

% cat = cat + 1;

toc;

%%  remove category

tic;

loc_rmcat( loc, all_cats );

toc;

%%  get categories / has categories

tic;
cats = loc_getcats( loc );
toc;

tic;
exists = loc_hascat( loc, cats );
toc;

%%  get labels

tic;
labs = loc_getlabs( loc );
toc;

%%  set category

tic;

for i = 1:1e3
    
sz = 1e5;
c_cat = uint32(cat-1);
label = uint32(i);
rows = uint32(sz);
assign_at = uint32( randperm(sz, floor(sz / 2)) );

loc_setcat( loc, c_cat, label, assign_at, rows );

end

toc;

% %%  find

tic;
inds = loc_find( loc, label );
toc;

assert( numel(inds) == numel(assign_at) );

assert(loc_size(loc) == sz);
