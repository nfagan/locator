function varargout = loc_from(loc_convertible)

%   LOC_FROM -- Convert to locator object.
%
%     IN:
%       - `loc_convertible` (struct, SparseLabels)
%     OUT:
%       - `varargout` (uint32)

if ( isa(loc_convertible, 'SparseLabels') )
  [loc, out_cats, out_labs] = from_sparse_labels( loc_convertible );
  varargout{1} = loc;
  varargout{2} = out_cats;
  varargout{3} = out_labs;
  return;
end

if ( isa(loc_convertible, 'struct') )
  varargout{1} = from_struct( loc_convertible );
  return;
end

error( 'Cannot convert to locator from object of type "%s".', class(loc_convertible) );

end

function loc = from_struct(s)

loc = loc_create();

try
  assert( isfield(s, 'indices'), 'Struct is missing `indices` field.' );
  assert( isfield(s, 'labels'), 'Struct is missing `labels` field.' );
  assert( isfield(s, 'categories'), 'Struct is missing `categories` field.' );
  
  assert( numel(s.labels) == numel(s.categories), 'Number of labels and indices must match.' );
  assert( numel(s.labels) == size(s.indices, 2), 'Number of labels must match columns of indices.' );
  
  assert( isnumeric(s.labels) && isnumeric(s.categories), 'Categories and labels must be numeric.' );
  assert( isa(s.indices, 'logical'), 'Indices must be logical.' );
  
  assert( numel(unique(s.labels)) == numel(s.labels), 'Labels must be unique.' );
  
  labs = uint32( s.labels );
  cats = uint32( s.categories );
  indices = s.indices;
  
  rows = size( indices, 1 );
  
  for i = 1:numel(labs)
    loc_requirecat( loc, cats(i) );
    inds = uint32( find(indices(:, i)) );
    loc_setcat( loc, cats(i), labs(i), inds, rows );
  end
  
catch err
  loc_destroy( loc );
  throw( err );
end

end

function [loc, out_cats, out_labs] = from_sparse_labels(sp)

loc = loc_create();

try
  in_inds = sp.indices;
  in_labs = sp.labels;
  in_cats = sp.categories;

  out_cats = containers.Map( 'keytype', 'char', 'valuetype', 'uint32' );
  out_labs = containers.Map( 'keytype', 'char', 'valuetype', 'uint32' );
  
  n_cats = uint32(1);

  for i = 1:size(in_inds, 2)

    in_lab = in_labs{i};
    in_cat = in_cats{i};
    
    out_lab = uint32(i);
    
    if ( ~out_cats.isKey(in_cat) )
      out_cats(in_cat) = n_cats;
      c_cat = n_cats;
      n_cats = n_cats + 1;
    else
      c_cat = out_cats(in_cat);
    end
    
    out_labs(in_lab) = out_lab;
    
    loc_requirecat( loc, c_cat );
    
    loc_setcat( loc, c_cat, out_lab, in_inds(:, i) );
  end
  
catch err
  loc_destroy( loc );
  throw( err );
end


end