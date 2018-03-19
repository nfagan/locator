function [I, C] = loc_findall_categorical(categ)

%   LOC_FINDALL_CATEGORICAL -- Produce output equivalent to loc_findall,
%     for a categorical matrix.
%
%     IN:
%       - `categ` (categorical)
%     OUT:
%       - `I` (double)
%       - `C` (categorical)

inp_msg = 'Input must be categorical matrix.';

assert( isa(categ, 'categorical'), inp_msg );
assert( ismatrix(categ), inp_msg );

[C, ~, ib] = unique( categ, 'rows' );
I = accumarray( ib, (1:numel(ib))', [], @(rows) {sort(rows)} );

end