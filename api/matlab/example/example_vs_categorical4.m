load census1994

is_categorical = varfun( @(x) isa(x, 'categorical'), adultdata, 'outputformat', 'uniform' );
cat_vars = adultdata.Properties.VariableNames(is_categorical);

cat_labs = adultdata{:, is_categorical};
to_labs = cellstr( cat_labs );
to_labs_no_undf = cellfun( @(x) strrep(x, '<undefined>', 'undefined'), to_labs, 'un', false );

[r, c] = find( strcmp(to_labs_no_undf, 'undefined') );

unq = unique( c );

for i = 1:numel(unq)
  ind = strcmp( to_labs_no_undf(:, unq(i)), 'undefined' );
  to_labs_no_undf(:, unq(i)) = { sprintf( 'undefined__%d', i) };
end


%%

tic;
labs = labeler.from( to_labs_no_undf, cat_vars );
toc;

%%

tic;
A = categorical( to_labs_no_undf );
toc;

%%
tic;
B = labeler.from( A, cat_vars );
toc;

%%

tic; [I, C] = findall( labs ); toc;

tic; [I2, C2] = loc_findall_categorical( A ); toc;

