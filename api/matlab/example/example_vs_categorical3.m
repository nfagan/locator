%%

addpath( '~/Documents/MATLAB/hannah_data/02222018/for_upload/' );
x = load( 'looking_duration.mat' );
data = x.looking_duration.data;
labels = x.looking_duration.labels;
sp = SparseLabels( labels );
labs = labeler.from( sp );
[m, f] = label_mat( sp );
categ = categorical( m );

%%

fprintf( '\n-- ' );

str_cats = { 'doses', 'gazes', 'expressions', 'sessions', 'file_names' };
% str_cats = f;

print_str = sprintf( '(findall) [%s]', strjoin(str_cats, ', ') );

fprintf( '\nlocator: %s\n', print_str );
tic;
[I, C] = findall( labs, str_cats );
toc;

str_cat_inds = cellfun( @(x) find(strcmp(f, x)), str_cats );
meas = categ(:, str_cat_inds);

fprintf( '\ncategorical: %s\n', print_str );
tic;
[C2, ~, ib] = unique( meas, 'rows' );
I2 = accumarray( ib, (1:numel(ib))', [], @(rows) {sort(rows)} );
toc;

fprintf( '\nSparseLabels: %s\n', print_str );
unq_sp_cats = unique( sp.categories );
unq_sp_inds = cellfun( @(x) find(strcmp(unq_sp_cats, x)), str_cats );
tic;
[I3, C3] = rget_indices( sp, unq_sp_cats(unq_sp_inds) );
toc;
%%

print_str = '(find some)';
fprintf( '\nlocator: %s\n', print_str );

tic;
for i = 1:1e3
  ind_loc = find( labs, {'ugit', 'ubit', 'ugin', 'ta080302'} );
end
toc;

print_str = '(find some)';
fprintf( '\ncategorical: %s\n', print_str );

tic;
images = categ(:, strcmp(f, 'images'));
sessions = categ(:, strcmp(f, 'sessions'));
for i = 1:1e3
  ind_cat = (images == 'ugit' | images == 'ubit' | images == 'ugin') & ...
    sessions == 'ta080302';
end
toc;

%%

labs = labeler.from( sp );

l1 = labeler();
l2 = labeler();

for i = 1:1e3
  j = randi( numel(labs), 1, 1 );
  z = keep( copy(labs), j );
  
  l1 = append( l1, z );
  l2 = append( l2, z );
  
  destroy( z );
end
