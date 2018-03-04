function varargout = loc_from(loc_convertible)

%   LOC_FROM -- Convert to locator object.
%
%     IN:
%       - `loc_convertible` (SparseLabels)
%     OUT:
%       - `varargout` (uint32)

if ( isa(loc_convertible, 'SparseLabels') )
  [loc, out_cats, out_labs] = from_sparse_labels( loc_convertible );
  varargout{1} = loc;
  varargout{2} = out_cats;
  varargout{3} = out_labs;
  return;
end

error( 'Cannot convert to locator from object of type "%s".', class(loc_convertible) );

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