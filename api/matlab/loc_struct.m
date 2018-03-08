function s = loc_struct(loc)

%   LOC_STRUCT -- Convert a locator to a matlab struct.
%
%     s = loc_struct( loc ) returns `s`, a 1x1 struct with `indices`,
%     `labels`, and `categories` fields.
%
%     `indices` is an MxN logical matrix of M rows by N unique labels.
%     `labels` is an Nx1 uint32 array of N unique labels; each `labels`(i)
%     is associated with the index `indices(:, i)`.
%     `categories` is an Nx1 uint32 array of N categories, where each
%     `categories(i)` is the category associated with each `labels(i)`.
%
%     IN:
%       - `loc` (uint32) -- Locator id.
%     OUT:
%       - `s` (struct)

labs = loc_getlabs( loc );

sz = loc_size( loc );

inds = false( sz, numel(labs) );

for i = 1:numel(labs)
  numeric_inds = loc_find( loc, labs(i) );
  
  inds(numeric_inds, i) = true;
end

s = struct();
s.indices = inds;
s.labels = labs;
s.categories = loc_whichcat( loc, labs );

end