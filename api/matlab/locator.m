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
      %     loc = locator() creates a new instance.
      %
      %     loc = locator( id ) constructs an object from the `id` of
      %     a valid instance.
      %
      %     See also locator/find, locator/findall, locator/initcat
      
      if ( nargin == 1 )
        if ( numel(id) ~= 1 || ~loc_isloc(id) )
          error( 'Invalid locator instance id given.' );
        end
        obj.id = uint32( id );
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
        if ( isvalid(obj) )
          sz = [ loc_size(obj.id), 1 ];
        else
          sz = [ 0, 1 ];
        end
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
      
      if ( isvalid(obj) )
        sz = loc_size( obj.id );
      else
        sz = 0;
      end
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
    
    function tf = isfullcat(obj, categories)
     
      %   ISFULLCAT -- True if a category has a label for each row.
      %
      %     If a locator is resized to a size greater than its current
      %     size, the indices for the additional rows are padded with
      %     `false`. In this way, for a given category, there are fewer
      %     labels than rows, and the category is not 'full'.
      %
      %     See also locator/resize, locator/combs
      %
      %     IN:
      %       - `categories` (uint32)
      %     OUT:
      %       - `tf` (logical)
      
      tf = loc_api( loc_opcodes('is_full_cat'), obj.id, uint32(categories) );      
    end
    
    function tf = trueat(obj, indices, rows)
      
      %   TRUEAT -- Assign true at indices.
      %
      %     tf = trueat(obj, [1, 2]); creates an Mx1 logical array `tf`
      %     with true values at indices [1, 2], and where M = numel(obj).
      %
      %     tf = trueat(...,  3) creates an array of size 3x1, instead of
      %     Mx1.
      %
      %     See also locator/setcat, locator/find
      %
      %     IN:
      %       - `indices` (uint32)
      %       - `rows` (uint32) |OPTIONAL|
      %     OUT:
      %       - `tf` (logical)
      
      if ( nargin < 3 )
        rows = numel( obj );
      end
      tf = false( rows, 1 );
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
    
    function [I, C] = findall(obj, categories)
      
      %   FINDALL -- Get indices of combinations of labels in categories.
      %
      %     I = findall( obj, [1, 3] ) returns a cell array of uint32
      %     indices `I`, where each index in I identifies a unique
      %     combination of labels in categories 1 and 3.
      %
      %     I = findall( obj ) finds all possible combinations of labels in
      %     all categories.
      %
      %     [I, C] = ... also returns `C`, an MxN matrix of M categories by
      %     N combinations, where each column `i` of C identifies the
      %     labels used to generate the i-th index of I.
      %
      %     See also locator/combs, locator/find
      %
      %     IN:
      %       - `categories` (uint32)
      %     OUT:
      %       - `I` (cell array of uint32)
      %       - `C` (uint32)
      
      if ( nargin < 2 )
        categories = getcats( obj );
      end
      
      [I, C] = loc_findall( obj.id, categories );
    end
    
    function [obj, I, C] = keepeach(obj, categories)
      
      %   KEEPEACH -- Retain combinations of labels in categories.
      %
      %     See also locator/findall
      %
      %     IN:
      %       - `categories` (uint32)
      %     OUT:
      %       - `I` (cell array of uint32)
      %       - `C` (cell array of uint32)
      
      if ( nargin < 2 )
        categories = getcats( obj );
      end
      
      [I, C] = loc_api( loc_opcodes('keep_each'), obj.id, uint32(categories) );
      
      if ( numel(categories) > 1 )
        C = reshape( C, numel(categories), numel(C) / numel(categories) );
      end
    end
    
    function cmbs = combs(obj, categories)
      
      %   COMBS -- Get present combinations of labels in categories.
      %
      %     C = combs( obj, [1, 3, 5] ) returns all present combinations of
      %     labels in categories 1, 3 and 5. `C` is a 3xN matrix of N
      %     combinations, where the i-th row of `C` corresponds to the i-th
      %     inputted category.
      %
      %     See also locator/find, locator/setcat, locator/initcat
      %
      %     IN:
      %       - `categories` (uint32)
      %     OUT:
      %       - `cmbs` (uint32)

      if ( nargin < 2 )
        categories = getcats( obj );
      end
      
      cmbs = loc_combs( obj.id, categories );      
    end
    
    function cts = count(obj, labels)
      
      %   COUNT -- Count the number of rows associated with a label(s).
      %
      %     IN:
      %       - `labels` (uint32)
      %     OUT:
      %       - `cts` (uint32)
      
      cts = loc_count( obj.id, labels );      
    end
    
    function N = nlabs(obj)
      
      %   NLABS -- Get the number of labels in the locator.
      %
      %     See also locator/count, locator/getlabs, locator/find
      %
      %     OUT:
      %       - `N` (uint32)
      
      N = loc_nlabs( obj.id );
    end
    
    function N = ncats(obj)
      
      %   NCATS -- Get the number of categories in the locator.
      %
      %     See also locator/count, locator/getlabs, locator/find
      %
      %     OUT:
      %       - `N` (uint32)
      
      N = numel( getcats(obj) );
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
    
    function mat = fullcat(obj, categories)
      
      %   FULLCAT -- Get the complete category(ies) in the locator.
      %
      %     See also locator/whichcat, locator/setcat
      %
      %     IN:
      %       - `categories` (uint32)
      
      mat = loc_fullcat( obj.id, categories );
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
    
    function lab = randlab(obj)
      
      %   RANDLAB -- Get a random label that does not already exist.
      %
      %     OUT:
      %       - `lab` (uint32)
      
      lab = loc_randlab( obj.id );     
    end
    
    function obj = swaplab(obj, from, to)
      
      %   SWAPLAB -- Change an existing label code to a new one.
      %
      %     An error is thrown if outgoing label does not exist, or if the
      %     incoming label already exists.
      %
      %     See also locator/locator, locator/swapcat
      %
      %     IN:
      %       - `from` (uint32)
      %       - `to` (uint32)
      
      loc_api( loc_opcodes('swap_lab'), obj.id, uint32(from), uint32(to) );      
    end
    
     function obj = swapcat(obj, from, to)
      
      %   SWAPCAT -- Change an existing category code to a new one.
      %
      %     An error is thrown if outgoing category does not exist, or if
      %     the incoming category already exists.
      %
      %     See also locator/locator, locator/swaplab
      %
      %     IN:
      %       - `from` (uint32)
      %       - `to` (uint32)
      
      loc_api( loc_opcodes('swap_cat'), obj.id, uint32(from), uint32(to) );      
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
    
    function labs = incat(obj, category)
      
      %   INCAT -- Get all labels in a category.
      %
      %     IN:
      %       - `category` (uint32)
      %     OUT:
      %       - `labs` (uint32)
      
      labs = loc_incat( obj.id, category );
    end
    
    function obj = setcat(obj, cat, label, index)
      
      %   SETCAT -- Assign label to category.
      %
      %     setcat( obj, 0, 10, true(100, 1) ); assigns the label 10 to
      %     category 0, associated with an all-true 100x1 index.
      %
      %     Rows at which `index` is true become false for other labels in
      %     the category. In this way, a row can only ever be identified by
      %     one label per category. 
      %
      %     If assigning to a label that already exists, the final index is 
      %     the result of an OR operation between the current and incoming 
      %     indices. E.g.:
      %
      %     obj = requirecat( locator(), 0 );
      %     setcat( obj, 0, 10, true(10, 1) );
      %     setcat( obj, 0, 10, false(10, 1) );
      %     find( obj, 10 ) % will be 1:10
      %     destroy( obj );
      %
      %     See also locator/requirecat, locator/whichcat
      %
      %     IN:
      %       - `cats` (uint32)
      %       - `label` (uint32)
      %       - `indices` (logical, uint32)
      %     OUT:
      %       - `loc` (uint32) -- Locator id.
      
      if ( numel(label) > 1 )
        if ( nargin < 4 )
          index = 1:numel( label );
        end
        loc_setcatmult( obj.id, cat, label, index );
      else
        loc_setcat( obj.id, cat, label, index );
      end
    end
    
    function obj = initcat(obj, cat, labels, sz)
      
      %   INITCAT -- Initialize a complete category.
      %
      %     initcat( obj, 0, 2 ) initializes the category 0 with the label
      %     2, associated with an all-true Mx1 index, where M is equal to
      %     numel(obj), or one, if the object is empty.
      %
      %     initcat( obj, 0, 1, 100 ) initializes the category 0 with the
      %     label 1, associated with an all-true 100x1 index. An error is
      %     thrown if the size of the locator is not 100.
      %
      %     IN:
      %       - `cat` (uint32)
      %       - `labels` (uint32)
      %       - `sz` (uint32)
      
      if ( nargin < 4 )
        if ( isempty(obj) )
          index = true( numel(labels), 1 );
        else
          index = true( numel(obj), 1 );
        end
      else
        index = true( sz, 1 );
      end
      
      addcat( obj, cat );
      
      try
        setcat( obj, cat, labels, index );
      catch err
        rmcat( obj, cat );
        throw( err );
      end
    end
    
    function obj = fillcat(obj, cat, lab)
      
      %   FILLCAT -- Assign full contents of category to label.
      %
      %     See also locator/setcat, locator/initcat, locator/combs
      %
      %     IN:
      %       - `cat` (uint32)
      %       - `lab` (uint32)
      
      loc_fillcat( obj.id, cat, lab );      
    end
    
    function obj = collapsecat(obj, cats)
      
      %   COLLAPSECAT -- Collapse category to single label.
      %
      %     collapsecat( obj, 0 ) replaces all labels in category 0 with a
      %     randomly generated label, unless category 0 has one or fewer
      %     labels to begin with.
      %
      %     See also locator/setcat, locator/requirecat
      %
      %     IN:
      %       - `cats` (uint32) -- Category(ies) to collapse.
        
      loc_collapsecat( obj.id, cats );
    end
    
    function obj = one(obj)
      
      %   ONE -- Retain a single row, collapsing non-uniform categories.
      %
      %     See also locator/collapsecat
      
      keep( collapsecat(obj, getcats(obj)), 1 );
    end
    
    function [full_cat, lab_map] = makelabs(obj, strs, lab_map)
      
      %   MAKELABS -- Make labels from cell array of strings.
      %
      %     full_cat = makecat( obj, {'hi', 'hello'} ); returns a 1x2 row
      %     vector with label ids corresponding to 'hi' and 'hello'.
      %
      %     [ ..., lab_map ] = makecat( ... ) also returns `lab_map`, a 
      %     multimap binding each unique char label to each unique integer 
      %     label.
      %
      %     ... = makecat( ..., lab_map ) puts the resulting int-char 
      %     label pairs into the multimap `lab_map`, instead of creating a 
      %     new multimap.
      %
      %     See also locator/initcat
      %
      %     IN:
      %       - `strs` (cell array of strings)
      %       - `lab_map` (multimap) |OPTIONAL|
      %     OUT:
      %       - `full_cat` (uint32)
      %       - `lab_map` (multimap)
      
      if ( nargin < 3 )
        lab_map = multimap();
        created_map = true;
      else
        assert( isa(lab_map, 'multimap'), 'Label map must be multimap.' );
        created_map = false;
      end
      
      assert( iscellstr(strs), 'Labels must be cell array of strings.' );
      
      strs = strs(:);      
      unqs = unique( strs );
      n = numel( unqs );
      full_cat = zeros( numel(strs), 1, 'uint32' );
      
      for i = 1:n
        lab_int = randlab( obj );
        lab_str = unqs{i};
        ind = strcmp( strs, lab_str );
        full_cat(ind) = lab_int;
        set( lab_map, lab_str, lab_int );
      end
      
      if ( nargout < 2 && created_map )
        destroy( lab_map );
      end
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
    
    function obj = resize(obj, to_size)
      
      %   RESIZE -- Resize the locator.
      %
      %     Resizing to a value smaller than the current size truncates
      %     indices to the new size.
      %
      %     Resizing to a value greater than the current size pads larger 
      %     indices with false values.
      %
      %     Resizing to 0 is equivalent to calling empty().
      %
      %     If the current size is 0, resizing has no effect.
      %
      %     See also locator/empty, locator/keep, locator.size
      %
      %     IN:
      %       - `to_size` (uint32)
      
      loc_resize( obj.id, to_size );
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
      
      cats = loc_getcats( id_ );
      
      if ( numel(cats) == 0 )
        addtl_str = 'with 0 categories';
      else
        addtl_str = 'with labels:';
      end
      
      max_labs = 5;
      
      sz = loc_size( id_ );
      sz_str = sprintf( '%d×1', sz );
      
      fprintf( '  %s %s %s', sz_str, link_str, addtl_str );
      
      if ( numel(cats) > 0 )
        fprintf( '\n' );
      end
      
      cat_strs = arrayfun( @(x) num2str(x), cats, 'un', false );
      n_digits = cellfun( @numel, cat_strs );
      
      max_n_digits = max( n_digits );
      
      for i = 1:numel(cats)
        c_cat = cats(i);
        labs = loc_incat( id_, c_cat );
        
        amt_pad = max_n_digits - numel( cat_strs{i} );
        cat_space = repmat( ' ', 1, amt_pad );
        
        n_labs = numel( labs );
        n_disp = min( n_labs, max_labs );
        
        if ( desktop_exists )
          fprintf( '\n  %s<strong>%d</strong>:', cat_space, c_cat );
        else
          fprintf( '\n  %s%d:', cat_space, c_cat );
        end
        
        lab_str = strjoin( arrayfun(@num2str, labs(1:n_disp), 'un', false), ', ' );
        
        if ( n_disp < n_labs )
          lab_str = sprintf( '%s ..', lab_str );
        end
        
        lab_str = sprintf( '[%s]', lab_str );
        
        fprintf( ' %s', lab_str );
      end
      
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
    
    
    function [dbl, cats] = double(obj)
      
      %   DOUBLE -- Convert the object to a double matrix.
      %
      %     mat = double( obj ) returns an MxN matrix of M label ids by N
      %     category ids, such that, for a given label i in category j,
      %     find( mat(:, j) == i ) will equal find( obj, i );
      %
      %     See also locator/locator, locator/find
      %
      %     OUT:
      %       - `dbl` (double)
      %       - `cats` (double)
      
      cats = getcats( obj );
      dbl = double( fullcat(obj, cats) );
      cats = double( cats );
    end
    
    function [categ, cats] = categorical(obj)
      
      %   CATEGORICAL -- Convert the object to a categorical array.
      %
      %     See also categorical, locator/double, locator/gather
      %
      %     OUT:
      %       - `cats` (categorical)
      
      [categ, cats] = double( obj );
      categ = categorical( categ );
    end
    
    function out = saveobj(obj)
      
      %   SAVEOBJ -- Save as struct.
      %
      %     OUT:
      %       - `out` (struct)
      
      out = gather( obj );
    end
  end 
  
  methods (Static = true)
    function varargout = from(varargin)
      
      %   FROM -- Convert to locator from compatible source.
      %
      %     See also loc_from, locator/locator
      
      [varargout{1:nargout}] = loc_from( varargin{:} );

      if ( ~isa(varargout{1}, 'locator') )
        varargout{1} = locator( varargout{1} );
      end
    end
    
    function s = undefined()
      
      %   UNDEFINED -- Return the undefined label.
      %
      %     OUT:
      %       - `s` (uint32)
      
      s = intmax( 'uint32' );      
    end
    
    function obj = loadobj(s)
      
      %   LOADOBJ -- Load from struct.
      %
      %     OUT:
      %       - `obj` (locator)
      
      obj = locator.from( s );
    end
    
    function loc = with(cat, label, index)
      
      %   WITH -- Create locator with category set to label.
      %
      %     See also locator/locator, locator/setcat
      %
      %     IN:
      %       - `cat` (uint32)
      %       - `label` (uint32)
      %       - `index` (logical)
      %     OUT:
      %       - `loc` (locator)
      
      if ( nargin < 3 )
        index = true( numel(label), 1 );
      end
      loc = locator();
      addcat( loc, cat );
      setcat( loc, cat, label, index );
    end
    
    function lab = randlab2(A, B)
      
      %   RANDLAB2 -- Get a label that does not exist between two locators.
      %
      %     IN:
      %       - `A` (locator)
      %       - `B` (locator)
      %     OUT:
      %       - `lab` (uint32)
      
      lab = loc_randlab2( A.id, B.id );
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
    
    function destroysome(ids)
      
      %   DESTROYSOME -- Destroy locators associated with ids.
      %
      %     See also locator/destroyall
      
      loc_destroy( ids );
    end
    
    function destroyexcept(keep_ids)
      
      %   DESTROYEXCEPT -- Destroy locators except those specified.
      %
      %     IN:
      %       - `keep_ids` (uint32)
      
      locator.destroysome( setdiff(locator.instances(), uint32(keep_ids)) );      
    end
  end
end