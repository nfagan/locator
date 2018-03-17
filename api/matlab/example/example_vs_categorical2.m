%%

cont = load( '~/Documents/MATLAB/hannah_data/02222018/for_upload/looking_duration.mat' );
cont = cont.looking_duration;
sp = SparseLabels( cont.labels );
sp = sp.repeat( 20 );
[m, f] = label_mat( sp );

%%
cont = get_example_container();
sp = cont.labels;
[m, f] = label_mat( sp );

%%

tic;
spc = categorical( m );
toc;

tic;
[loc, c, l] = locator.from( sp );
toc;

%%

% desired_cats = { 'doses', 'images', 'file_names', 'genders', 'sessions' };
desired_cats = f(1);

tic;
x = combs( loc, c(desired_cats) );
toc;
%%

cat_ind = cellfun( @(x) find(strcmp(f, x)), desired_cats );

tic;
meas = spc(:, cat_ind);
[C, ~, ib] = unique( meas, 'rows' );
numoccurences = accumarray(ib, 1);
I = accumarray( ib, (1:numel(ib))', [], @(rows) {sort(rows)} );
disp( toc * 1e3 );
%%
for i = 1:numel(I)
  subset_row = cellstr( C(i, :) );
  matching_l = l(subset_row);
  assert( isequal(find(loc, matching_l), I{i}) );
end

%%
tic;
[I2, C2] = findall( loc, c(desired_cats) );
disp( toc * 1e3 );

%%

tic;
[I, C] = findall( loc );

data = cont.data;

for i = 1:numel(I)
  subset_data = data(I{i}, :);
end

toc;
%%
tic;
y = unique( spc, 'rows' );
for i = 1:size(y, 1)
  ind = ...
    spc(:, 1) == y(i, 1) & ...
    spc(:, 2) == y(i, 2) & ...
    spc(:, 3) == y(i, 3) & ...
    spc(:, 4) == y(i, 4) & ...
    spc(:, 5) == y(i, 5) & ...
    spc(:, 6) == y(i, 6);
  
  subset_data = data(I{i}, :);
end
toc;

%%

