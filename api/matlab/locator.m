classdef locator
  
  properties (Access = private)
    id;
  end
  
  methods (Access = public)
    function obj = locator(id)
      
      %   LOCATOR -- Construct a locator object.
      %
      %     LOCATOR objects are meant to quickly generate indices of
      %     data labels and label combinations, in the vein of categorical
      %     arrays.
      %
      %     The LOCATOR class is a high-level wrapper around the loc_*.m
      %     series of functions; consult their documentation for further
      %     information.
      %
      %     See also locator/find, locator/addcat, locator/setcat
      
      if ( nargin == 1 )
        assert( numel(id) == 1 && loc_isloc(id) ...
          , 'Invalid locator instance id given.', id );
        obj.id = id;
      else
        obj.id = loc_create();
      end
    end
    
    function tf = isvalid(obj)
      
      %   ISVALID -- True if the object has not been destroyed.
      %
      %     See also locator/destroy
      %
      %     OUT:
      %       - `tf` (logical)
      
      tf = loc_isloc( obj.id );
    end
    
    function n = numel(obj)
      
      %   NUMEL -- Get the number of rows in the locator.
      %
      %     See also locator/size, locator/getcats
      %
      %     OUT:
      %       - `n` (uint32)
      
      n = loc_size( obj.id );      
    end
    
    function sz = size(obj, dim)
      
      %   SIZE -- Get the number of rows in the locator.
      %
      %     See also locator/numel, locator/getlabs
      %
      %     IN:
      %       - `dimension` |OPTIONAL| (numeric)
      %     OUT:
      %       - `sz` (uint32)
      
      if ( nargin == 1 )
        sz = [ loc_size(obj.id), 1 ];
        return;
      end
      
      msg = [ 'Dimension argument must be a positive integer' ...
          , ' scalar within indexing range.' ];
        
      if ( ~isnumeric(dim) || ~isscalar(dim) || dim < 1 )
        error( msg );
      end
      
      if ( dim > 1 )
        sz = 1;
        return;
      end
      
      sz = loc_size( obj.id );
    end
    
    function tf = isempty(obj)
      
      %   ISEMPTY -- True if the object has no labels.
      %
      %     See also locator/size, locator/empty
      %
      %     OUT
      %       - `tf` (logical)
      
      tf = loc_isempty( obj.id );
    end
    
    function tf = trueat(obj, indices)
      
      %   TRUEAT -- Assign true at indices.
      %
      %     tf = trueat(obj, [1, 2]); creates an Mx1 logical array `tf`
      %     with true values at indices [1, 2], and where M = numel(obj).
      %
      %     IN:
      %       - `indices` (uint32)
      %     OUT:
      %       - `tf` (logical)
      
      tf = false( size(obj) );
      tf(indices) = true;
    end
    
    function inds = find(obj, labels)
      
      %   FIND -- Get indices associated with labels.
      %
      %     Within a category, indices are calculated via an `or` operation.
      %     Across categories, indices are calculated via an `and` operation.
      %
      %     E.g., if `obj` is a locator with labels 0 and 1 in category 0,
      %     then find( obj, [0, 1] ) returns rows associated with 0 OR 1.
      %
      %     But if `obj` is a locator with labels 0 and 1 in categories 0
      %     and 1, respectively, then find( obj, [0, 1] ) returns the 
      %     subset of rows associated with 0 AND 1.
      %
      %     See also locator/getlabs, locator/getcats
      %
      %     IN:
      %       - `labels` (uint32)
      %     OUT:
      %       - `inds` (uint32)
      
      inds = loc_find( obj.id, labels );
    end
    
    function obj = addcat(obj, category)
      
      %   ADDCAT -- Add a category or categories to the locator.
      %
      %     See also locator/getcats, locator/setcat
      %
      %     IN:
      %       - `category` (uint32)
      
      loc_addcat( obj.id, category );
    end
    
    function obj = rmcat(obj, categories)
      
      %   RMCAT -- Remove a category or categories.
      %
      %     See also locator/requirecat, locator/setcat
      %
      %     IN:
      %       - `category` (uint32)
      
      loc_rmcat( obj.id, categories );
    end
    
    function obj = requirecat(obj, category)
      
      %   REQUIRECAT -- Add category if it does not exist.
      %
      %     See also locator/getcats, locator/setcat, locator/getlab
      %
      %     IN:
      %       - `category` (uint32)
      
      loc_requirecat( obj.id, category );
    end
    
    function tf = hascat(obj, category)
      
      %   HASCAT -- True if the category exists.
      %
      %     See also locator/haslab, locator/find
      %
      %     IN:
      %       - `category` (uint32)
      %     OUT:
      %       - `tf` (logical)
      
      tf = loc_hascat( obj.id, category );      
    end
    
    function tf = haslab(obj, label)
      
      %   HASLAB -- True if the label exists.
      %
      %     See also locator/whichcat
      %
      %     IN:
      %       - `label` (uint32)
      %     OUT:
      %       - `tf` (logical)
      
      tf = loc_haslab( obj.id, label );
    end
    
    function cats = whichcat(obj, labels)
      
      %   WHICHCAT -- Get the category in which a label resides.
      %
      %     See also locator/hascat, locator/find
      %
      %     IN:
      %       - `labels` (uint32) -- Labels to search.
      %     OUT:
      %       - `cats` (uint32) -- Categories.
      
      cats = loc_whichcat( obj.id, labels );
    end
    
    function obj = setcat(obj, cat, label, index)
      
      %   SETCAT -- Assign label to category.
      %
      %     setcat( obj, 0, 10, true(100, 1) ); assigns the label 10 to
      %     category 0, associated with an all-true 100x1 index.
      %
      %     See also locator/requirecat, locator/whichcat
      %
      %     IN:
      %       - `cats` (uint32)
      %       - `label` (uint32)
      %       - `indices` (logical, uint32)
      %     OUT:
      %       - `loc` (uint32) -- Locator id.
      
      loc_setcat( obj.id, cat, label, index );
    end
    
    function cats = getcats(obj)
      
      %   GETCATS -- Get the categories in the locator.
      %
      %     See also locator/getlabs, locator/find
      %
      %     OUT:
      %       - `cats` (uint32) -- Categories.
      
      cats = loc_getcats( obj.id );
    end
    
    function labs = getlabs(obj)
      
      %   GETLABS -- Get the labels in the locator.
      %
      %     See also locator/getcats, locator/whichcat
      %
      %     OUT:
      %       - `labs` (uint32) -- Labels.
      
      labs = loc_getlabs( obj.id );
    end
    
    function id_ = getid(obj)
      
      %   GETID -- Return the handle to the locator instance.
      %
      %     OUT:
      %       - `id` (uint32)
      
      id_ = obj.id;
    end
    
    function b = copy(obj)
      
      %   COPY -- Create a copy of the locator instance.
      %
      %     See also locator/locator, locator/destroy
      %
      %     OUT:
      %       - `b` (locator)
      
      id_b = loc_copy( obj.id );
      b = locator( id_b );
    end
    
    function obj = append(obj, b)
      
      %   APPEND -- Append one locator to another.
      %
      %     loca = append( a, b ) appends the contents of `b` to `a`.
      %
      %     An error is thrown if the categories of `a` do not match those 
      %     of `b`.
      %
      %     See also locator/keep, locator/only
      %
      %     IN:
      %       - `b` (locator)
      
      loc_append( obj.id, b.id );
    end
    
    function obj = keep(obj, at)
      
      %   KEEP -- Retain rows at indices.
      %
      %     keep( obj, [1, 3, 5] ); retains the 1st, 3rd, and 5th rows of
      %     `obj`.
      %
      %     See also locator/find, locator/append, locator/only
      %
      %     IN:
      %       - `at` (uint32) -- Indices
      
      loc_keep( obj.id, at );
    end
    
    function obj = only(obj, labels)
      
      %   ONLY -- Retain rows associated with labels.
      %
      %     See also locator/find, locator/keep
      %
      %     IN:
      %       - `labels` (uint32)
      
      loc_keep( obj.id, loc_find(obj.id, labels) );
    end
    
    function obj = empty(obj)
      
      %   EMPTY -- Retain 0 rows, but keep categories.
      %
      %     See also locator/keep, locator/clear
      
      loc_empty( obj.id );
    end
    
    function obj = clear(obj)
      
      %   CLEAR -- Clear contents.
      %
      %     See also locator/keep, locator/clear
      
      loc_clear( obj.id );
    end
    
    function tf = eq(obj, b)
      
      %   EQ -- True if two locators have equal contents.
      %
      %     See also locator/ne, locator/find
      %
      %     IN:
      %       - `b` (/any/)
      %     OUT:
      %       - `tf` (logical)
      
      tf = false;
      
      if ( ~isa(obj, 'locator') || ~isa(b, 'locator') )
        return;
      end
      
      tf = loc_eq( obj.id, b.id );
    end
    
    function tf = ne(obj, b)
      
      %   NE -- True if two locators have unequal contents.
      %
      %     See also locator/eq
      %
      %     IN:
      %       - `b` (/any/)
      %     OUT:
      %       - `tf` (logical)
      
      tf = ~eq( obj, b );
    end
    
    function destroy(obj)
      
      %   DESTROY -- Delete the object and free memory.
      %
      %     See also locator/locator, locator/copy
      
      loc_destroy( obj.id );
    end
    
    function disp(obj)
      
      %   DISP -- Pretty-print the object's contents.
      %
      %     See also locator/locator, locator/getcats
      
      id_ = obj.id;
      
      desktop_exists = usejava( 'desktop' );
      
      if ( desktop_exists )
        link_str = sprintf( '<a href="matlab:helpPopup %s">%s</a>' ...
          , class(obj), class(obj) );
      else
        link_str = class( obj );
      end
      
      if ( ~loc_isloc(id_) )
        fprintf( 'Handle to deleted %s instance.\n\n', link_str );
        return;
      end
      
      sz = loc_size( id_ );
      cats = loc_getcats( id_ );
      labs = loc_getlabs( id_ );

      sz_str = sprintf( '%d×1', sz );

      n_cats = numel( cats );
      n_labs = numel( labs );

      if ( n_cats == 1 )
        cat_str = 'category';
      else
        cat_str = 'categories';
      end

      if ( n_labs == 1 )
        lab_str = 'label';
      else
        lab_str = 'labels';
      end

      fprintf( '  %s %s', sz_str, link_str );
      fprintf( '\n\n  %d %s, %d %s', n_cats, cat_str, n_labs, lab_str );
      fprintf( '\n\n' );
    end
    
    function s = gather(obj)
      
      %   GATHER -- Copy contents to a Matlab struct.
      %
      %     See also locator/from
      %
      %     OUT:
      %       - `s` (struct)
      
      s = loc_struct( obj.id );
    end
  end 
  
  methods (Static = true)
    function varargout = from(loc_convertible)
      
      %   FROM -- Create locator from compatible source.
      %
      %     See also loc_from, locator/locator
      
      nargoutchk( 1, Inf );
      
      [varargout{1:nargout}] = loc_from( loc_convertible );
      
      varargout{1} = locator( varargout{1} );
    end
    
    function out = instances(varargin)
      
      %   INSTANCES -- Get all active locator ids.
      %
      %     See also loc_create, locator/destroyall
      %
      %     OUT:
      %       - `out` (uint32)
      
      out = loc_instances(varargin{:});
    end
    
    function destroyall()
      
      %   DESTROYALL -- Destroy all active locators.
      %
      %     See also locator/locator, loc_create, locator/instances
      
      loc_destroy();
    end
  end
end