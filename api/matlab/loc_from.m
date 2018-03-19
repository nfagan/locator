function varargout = loc_from(varargin)

%   LOC_FROM -- Convert to locator object.
%
%     IN:
%       - `loc_convertible` (struct, SparseLabels)
%     OUT:
%       - `varargout` (uint32)

narginchk( 1, 2 );

loc_convertible = varargin{1};

if ( isa(loc_convertible, 'SparseLabels') )
  [loc, out_cats, out_labs] = from_sparse_labels( loc_convertible );
  varargout{1} = loc;
  varargout{2} = out_cats;
  varargout{3} = out_labs;
  return;
end

if ( isa(loc_convertible, 'struct') )
  fields = fieldnames( loc_convertible );
  if ( numel(fields) > 0 && iscellstr(loc_convertible.(fields{1})) )
    [loc, out_cats, out_labs] = from_struct_cell( loc_convertible );
    varargout{1} = loc;
    varargout{2} = out_cats;
    varargout{3} = out_labs;
    return;
  else
    varargout{1} = from_struct( loc_convertible );
    return;
  end
end

if ( isa(loc_convertible, 'cell') || isa(loc_convertible, 'categorical') )
  if ( nargin == 1)
    error( ['If converting from a cell array of strings or categorical array' ...
      , ' provide category names for the columns of the array as a second input.'] );
  end
  
  narginchk( 2, 2 );
  
  [loc, out_cats, out_labs] = from_cellstr( cellstr(loc_convertible) ...
    , cellstr(varargin{2}) );
  
  varargout{1} = loc;
  varargout{2} = out_cats;
  varargout{3} = out_labs;
  
  return;
end

error( 'Cannot convert to locator from object of type "%s".', class(loc_convertible) );

end

function [loc, out_cats, out_labs] = from_cellstr(mat, cats)

assert( ismatrix(mat), 'Cell array of strings must be a 2-d matrix.' );
assert( size(mat, 2) == numel(cats), 'Expected %d category names; %d were given.' ...
  , size(mat, 2), numel(cats) );

assert( iscellstr(mat) && iscellstr(cats), 'Cell inputs must be cell array of strings.' );

c_loc_instances = locator.instances();
c_m_instances = multimap.instances();

try

  loc = locator();
  out_cats = multimap();
  out_labs = multimap();

  lab_stp = 1;

  N = size( mat, 2 );

  for i = 1:N
    if ( i > 1 && contains(out_cats, cats{i}) )
      error( 'Category names must be unique.' );
    end

    if ( numel(cats{i}) == 0 )
      error( 'Category name cannot be empty.' );
    end

    curr = mat(:, i);
    unqs = unique( curr );

    initcat( loc, i, randlab(loc), size(curr, 1) );

    set( out_cats, cats{i}, i );

    for j = 1:numel(unqs)
      ind = strcmp( curr, unqs{j} );
      
      if ( numel(unqs{j}) == 0 )
        error( 'Label cannot be empty.' );
      end

      setcat( loc, i, lab_stp, ind );

      if ( contains(out_labs, unqs{j}) )

        other_cat = get( out_cats, whichcat(loc, out_labs(unqs{j})) );

        error( 'The label "%s" was already present in category "%s".' ...
          , unqs{j}, other_cat );
      end

      set( out_labs, unqs{j}, lab_stp );

      lab_stp = lab_stp + 1;
    end
  end
catch err
  locator.destroyexcept( c_loc_instances );
  multimap.destroyexcept( c_m_instances );
  throw( err );
end


end

function [loc, out_cats, out_labs] = from_struct_cell(loc_convertible)

fields = fieldnames( loc_convertible );

for i = 1:numel(fields)
  curr = loc_convertible.(fields{i});
  assert( iscellstr(curr), 'If a field of struct is cell, all must be cell array of strings.' );
  if ( i > 1 )
    assert( numel(loc_convertible.(fields{i-1})) == numel(curr) ...
      , 'All fields must have the same number of elements.' );
  end
end

c_loc_instances = locator.instances();
c_m_instances = multimap.instances();

try
  loc = locator();
  out_cats = multimap();
  out_labs = multimap();

  lab_stp = 1;

  for i = 1:numel(fields)
    curr = loc_convertible.(fields{i});
    unqs = unique( curr );

    initcat( loc, i, randlab(loc), size(curr, 1) );

    set( out_cats, fields{i}, i );

    for j = 1:numel(unqs)
      ind = strcmp( curr, unqs{j} );

      setcat( loc, i, lab_stp, ind );

      if ( contains(out_labs, unqs{j}) )

        other_cat = get( out_cats, whichcat(loc, out_labs(unqs{j})) );

        destroy( loc );
        destroy( out_cats );
        destroy( out_labs );

        error( 'The label "%s" was already present in category "%s".' ...
          , unqs{j}, other_cat );
      end

      set( out_labs, unqs{j}, lab_stp );

      lab_stp = lab_stp + 1;
    end
  end

catch err 
  locator.destroyexcept( c_loc_instances );
  locator.destroyexcept( c_m_instances );
  
  throw( err );
end

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
  
  out_cats = multimap();
  out_labs = multimap();
  
  n_cats = uint32(1);

  for i = 1:size(in_inds, 2)

    in_lab = in_labs{i};
    in_cat = in_cats{i};
    
    out_lab = uint32(i);
    
    if ( ~contains(out_cats, in_cat) )
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