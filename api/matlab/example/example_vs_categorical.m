hospital = load( 'hospital.mat' );

hospital = dataset2cell( hospital.hospital );

all_cats = hospital(1, :);

is_convertible = cellfun( @(x) ischar(x) | isa(x, 'nominal'), hospital(2, :) );
char_hospital = cellfun( @char, hospital(2:end, is_convertible), 'un', false );
cats = all_cats(is_convertible);

%%
tic;

loc = locator();
l = multimap();
c = multimap();

for i = 1:numel(cats)
  set( c, cats{i}, i );
  initcat( loc, i, makelabs(loc, char_hospital(:, i), l) );
end

toc;

%%

tic; 

[I, C] = findall( loc, c({'Sex', 'LastName'}) );

toc;


%%

tic; A = categorical( char_hospital ); toc;

%%

last_names = A(:, 2);

tic; for i = 1:1e5, y = last_names == 'TURNER' | last_names == 'DIAZ'; end; toc;

%%

search_for = l({'TURNER', 'DIAZ'});

tic; for i = 1:1e5, y = find( loc, search_for ); end; toc;


