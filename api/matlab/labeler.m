classdef labeler
  
  properties (Access = private, Constant = true)
    COLLAPSED_PATTERN = '<%s>';
  end
  
  properties (Access = private)
    loc;
    categories;
    labels;
  end
  
  methods
    function obj = labeler(loc, categories, labels)
      
      %   LABELER -- Construct a labeler object.
      %
      %     A LABELER is an aggregate structure of locator and multimap
      %     instances. The categories and labels in the LABELER should
      %     correspond to those in its locator, in which case common
      %     operations like find() and findall() can take char identifiers,
      %     in addition to uint32 identifiers, as input.
      %
      %     See also locator/locator, multimap/multimap
      
      if ( nargin == 0 )
        obj.loc = locator();
        obj.categories = multimap();
        obj.labels = multimap();
        return;
      end
      
      assert( isa(loc, 'locator'), 'Input 1 must be a locator instance.' );
      assert( isa(categories, 'multimap'), 'Input 2 must be a multimap instance.' );
      assert( isa(labels, 'multimap'), 'Input 3 must be a multimap instance.' );
      
      obj.loc = loc;
      obj.categories = categories;
      obj.labels = labels;
    end
    
    function tf = isempty(obj)
      
      %   ISEMPTY -- True if the object is of size 0.
      %
      %     See also locator/size, locator/empty
      %
      %     OUT
      %       - `tf` (logical)
      
      tf = isempty( obj.loc );
    end
    
    function tf = isvalid(obj)
      
      %   ISVALID -- True if all properties are valid instances.
      %
      %     See also locator/isvalid, multimap/isvalid
      %
      %     OUT:
      %       - `tf` (logical)
      
      tf = isvalid( obj.loc ) && isvalid( obj.labels ) && isvalid( obj.categories );      
    end
    
    function tf = eq(obj, B)
      
      %   EQ -- True if two objects are labelers with identical labels,
      %     categories, and locators.
      %
      %     See also locator/eq
      %
      %     IN:
      %       - `B` (labeler)
      %     OUT:
      %       - `tf` (logical)
      
      tf = false;
      
      if ( ~isa(obj, 'labeler') || ~isa(B, 'labeler') )
        return;
      end
      
      if ( obj.loc == B.loc )
        tf = true;
        return;
      end
      
      if ( numel(obj) ~= numel(B) )
        return;
      end
      
      cats_a = sort( strcats(obj) );
      cats_b = sort( strcats(B) );
      
      if ( ~isequal(cats_a, cats_b) )
        return;
      end
      
      labs_a = sort( strlabs(obj) );
      labs_b = sort( strlabs(B) );
      
      if ( ~isequal(labs_a, labs_b) )
        return;
      end
      
      for i = 1:numel(labs_a)
        ind_a = find( obj, labs_a{i} );
        ind_b = find( B, labs_a{i} );
        
        if ( ~isequal(ind_a, ind_b) )
          return;
        end
      end
      
      tf = true;
    end
    
    function tf = ne(obj, B)
      
      %   NE -- True if two objects are not labelers with identical labels,
      %     categories, and locators.
      %
      %     See also locator/eq
      %
      %     IN:
      %       - `B` (labeler)
      %     OUT:
      %       - `tf` (logical)
      
      tf = ~eq( obj, B );
    end
    
    function sz = size(obj, varargin)
      
      %   SIZE -- Get the number of rows in the labeler.
      %
      %     See also locator/size
      %
      %     OUT:
      %       - `sz` (uint32)
      
      sz = size( obj.loc, varargin{:} );
    end
    
    function sz = numel(obj)
      
      %   NUMEL -- Get the number of rows in the labeler.
      %
      %     See also labeler/size
      %
      %     OUT:
      %       - `sz` (uint32)
      
      sz = numel( obj.loc );
    end
    
    function B = copy(obj)
      
      %   COPY -- Create a copy of the labeler instance.
      %
      %     See also labeler/labeler
      %
      %     OUT:
      %       - `B` (labeler)
      
      loc_b = copy( obj.loc );
      cat_b = copy( obj.categories );
      lab_b = copy( obj.labels );
      
      B = labeler( loc_b, cat_b, lab_b );
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
      
      keep( obj.loc, at );
      missing_labs = setdiff( values(obj.labels), getlabs(obj.loc) );
      remove( obj.labels, missing_labs );
    end
    
    function obj = only(obj, labels)
      
      %   ONLY -- Retain rows associated with labels.
      %
      %     See also locator/find, locator/keep
      %
      %     IN:
      %       - `labels` (uint32, char, cell array of strings)
      
      keep( obj, find(obj, labels) );
    end
    
    function n = nlabs(obj)
      
      %   NLABS -- Get the number of unique labels in the labeler.
      %
      %     See also locator/nlabs
      
      n = nlabs( obj.loc );      
    end
    
    function n = ncats(obj)
      
      %   NCATS -- Get the number of categories in the labeler.
      %
      %     See also locator/ncats
      
      n = numel( obj.categories );
    end
    
    function n = count(obj, labels)
      
      %   COUNT -- Count the number of rows associated with labels.
      %
      %     IN:
      %       - `label` (uint32, char)
      %     OUT:
      %       - `n` (uint32)
      
      if ( ~isnumeric(labels) )
        labels = tryget( obj.labels, labels );
      end
      
      n = count( obj.loc, labels );
    end
    
    function c = getcats(obj)
      
      %   GETCATS -- Get the numeric categories in the labeler.
      %
      %     See also labeler/getlabs
      %
      %     OUT:
      %       - `c` (uint32)
      
      c = getcats( obj.loc );      
    end
    
    function l = getlabs(obj)
      
      %   GETLABS -- Get the numeric labels in the labeler.
      %
      %     See also labeler/getcats
      %
      %     OUT:
      %       - `l` (uint32)
      
      l = getlabs( obj.loc );
    end
    
    function c = strcats(obj)
      
      %   STRCATS -- Get the char categories in the labeler.
      %
      %     See also labeler/getcats
      %
      %     OUT:
      %       - `c` (cell array of strings)
      
      c = keys( obj.categories );      
    end
    
    function l = strlabs(obj)
      
      %   STRLABS -- Get the char labels in the labeler.
      %
      %     See also labeler/getcats
      %
      %     OUT:
      %       - `c` (cell array of strings)
      
      l = keys( obj.labels );
    end
    
    function l = maplabs(obj, labels)
      
      %   MAPLABS -- Get values bound to label ids.
      %
      %     See also labeler/mapcats, labeler/findall
      %
      %     IN:
      %       - `labels` (uint32, char)
      %     OUT:
      %       - `l` (uint32, char)
      
      l = tryget( obj.labels, labels );
    end
    
    function c = mapcats(obj, categories)
      
      %   MAPCATS -- Get values bound to category ids.
      %
      %     See also labeler/maplabs, labeler/labeler
      %
      %     IN:
      %       - `categories` (uint32, char)
      %     OUT:
      %       - `c` (uint32, char)
      
      c = tryget( obj.categories, categories );
    end
    
    function l = incat(obj, category)
      
      %   INCAT -- Get the numeric labels in a category.
      %
      %     See also locator/incat
      %
      %     IN:
      %       - `category` (uint32, char)
      %     OUT:
      %       - `l` (uint32)
      
      if ( ~isnumeric(category) )
        category = tryget( obj.categories, category );
      end
      
      l = incat( obj.loc, category );      
    end
    
    function c = whichcat(obj, labels)
      
      %   WHICHCAT -- Get the category in which a label resides.
      %
      %     See also locator/hascat, locator/find
      %
      %     IN:
      %       - `labels` (uint32, char, cell array of strings)
      %     OUT:
      %       - `cats` (char, cell array of strings)
      
      if ( ~isnumeric(labels) )
        labels = tryget( obj.labels, labels );
      end
      
      c = whichcat( obj.loc, labels );      
    end
    
    function s = strwhichcat(obj, labels)
      
      %   STRWHICHCAT -- Get the char category in which a label resides.
      %
      %     See also locator/hascat, locator/find
      %
      %     IN:
      %       - `labels` (uint32, char, cell array of strings)
      %     OUT:
      %       - `cats` (char, cell array of strings)
      
      s = tryget( obj.categories, whichcat(obj, labels) );
    end
    
    function obj = collapsecat(obj, categories)
      
      %   COLLAPSECAT -- Collapse category to single label.
      %
      %     See also locator/collapsecat
      %
      %     IN:
      %       - `categories` (uint32, char, cell array of strings)
      
      if ( ~isnumeric(categories) )
        categories = get( obj.categories, categories );
      end
      
      for i = 1:numel(categories)
        cat_id = categories(i);
        
        c_incat = incat( obj.loc, cat_id );
        
        if ( numel(c_incat) > 1 )
          lab_id = randlab( obj.loc );
          remove( obj.labels, c_incat );
          str_cat = get( obj.categories, cat_id );
          set( obj.labels, sprintf(obj.COLLAPSED_PATTERN, str_cat), lab_id );
          fillcat( obj.loc, cat_id, lab_id );
        end
      end
    end
    
    function obj = one(obj)
      
      %   ONE -- Retain a single row, collapsing non-uniform categories.
      %
      %     See also locator/collapsecat
      
      keep( collapsecat(obj, getcats(obj)), 1 );
    end
    
    function C = combs(obj, categories)
      
      %   COMBS -- Get present combinations of labels in categories.
      %
      %     See also locator/combs
      %
      %     IN:
      %       - `categories` (uint32, char, cell array of strings)
      %     OUT:
      %       - `cmbs` (uint32)
      
      if ( nargin == 1 )
        C = combs( obj.loc, getcats(obj.loc) );
        return;
      end
      
      if ( ~isnumeric(categories) )
        categories = tryget( obj.categories, categories );
      end
      
      C = combs( obj.loc, categories );
    end
    
    function C = strcombs(obj, varargin)
      
      %   STRCOMBS -- Get combinations of labels in categories, as char.
      %
      %     See also labeler/combs
      %
      %     IN:
      %       - `categories` (uint32, char, cell array of strings)
      %     OUT:
      %       - `cmbs` (uint32)
      
      C = tryget( obj.labels, combs(obj, varargin{:}) );
    end
    
    function I = find(obj, labels)
      
      %   FIND -- Get indices associated with labels.
      %
      %     See also locator/find
      %
      %     IN:
      %       - `labels` (uint32, char, cell array of strings)
      %     OUT:
      %       - `inds` (uint32)
      
      if ( ~isnumeric(labels) )
        I = find( obj.loc, tryget(obj.labels, labels) );
        return;
      end
      
      I = find( obj.loc, labels );
    end
    
    function [I, C] = findall(obj, categories)
      
      %   FINDALL -- Get indices of combinations of labels in categories.
      %
      %     See also locator/findall
      %
      %     IN:
      %       - `categories` (uint32, char, cell array of strings)
      %     OUT:
      %       - `I` (uint32)
      %       - `C` (uint32)
      
      if ( nargin == 1 )
        [I, C] = findall( obj.loc, getcats(obj.loc) );
        return;
      end
      
      if ( ~isnumeric(categories) )
        [I, C] = findall( obj.loc, tryget(obj.categories, categories) );
        return;
      end
      
      [I, C] = findall( obj.loc, categories );
    end
    
    function obj = append(obj, B)
      
      %   APPEND -- Append one labeler to another.
      %
      %     See also locator/append
      %
      %     IN:
      %       - `B` (labeler)
      
      if ( ~isa(obj, 'labeler') )
        error( 'Cannot append objects of class "%s".', class(obj) );
      elseif ( ~isa(B, 'labeler') )
        error( 'Cannot append objects of class "%s".', class(B) );
      end
      
      if ( isempty(B) )
        return;
      end
      
      if ( isempty(obj) )
        destroy( obj );
        obj = copy( B );
        return;
      end
      
      if ( ncats(obj) ~= ncats(B) )
        error( 'Categories do not match.' );
      end
      
      str_cats_a = sort( get(obj.categories, getcats(obj.loc)) );
      str_cats_b = sort( get(B.categories, getcats(B.loc)) );
      
      if ( ~all(strcmp(str_cats_a, str_cats_b)) )
        error( 'Categories do not match.' );
      end
      
      B = copy( B );
      
      n_cats = numel( str_cats_a );
      
      %   make sure category ids match between labelers.
      %   if they don't, make them match.
      for i = 1:n_cats
        str_cat = str_cats_a{i};
        num_cat_a = get( obj.categories, str_cat );
        num_cat_b = get( B.categories, str_cat );
        
        if ( num_cat_a ~= num_cat_b )
          new_cat_id = locator.randlab2( obj.loc, B.loc );
          
          swapcat( obj.loc, num_cat_a, new_cat_id );
          swapcat( B.loc, num_cat_b, new_cat_id );
          
          set( obj.categories, str_cat, new_cat_id );
        end
      end
      
      str_labs_a = sort( strlabs(obj) );
      str_labs_b = sort( strlabs(B) );
      
      shared_str_labs = intersect( str_labs_a, str_labs_b );
      new_str_labs = setdiff( str_labs_b, str_labs_a );
      
      n_shared = numel( shared_str_labs );
      n_new = numel( new_str_labs );
      
      % ensure labels reside in consistent categories
      for i = 1:n_shared
        str_lab = shared_str_labs{i};
        
        cat_a = whichcat( obj.loc, get(obj.labels, str_lab) );
        cat_b = whichcat( B.loc, get(B.labels, str_lab) );
        
        str_cat_a = get( obj.categories, cat_a );
        str_cat_b = get( B.categories, cat_b );
        
        if ( ~strcmp(str_cat_a, str_cat_b) )
          destroy( B );
          error( 'Labels shared between labelers must reside in the same category.' );
        end
        
        num_lab_a = get( obj.labels, str_lab );
        num_lab_b = get( B.labels, str_lab );
        
        if ( num_lab_a ~= num_lab_b )
          new_lab_id = locator.randlab2( obj.loc, B.loc );
          
          swaplab( obj.loc, num_lab_a, new_lab_id );
          swaplab( B.loc, num_lab_b, new_lab_id );
          
          set( obj.labels, str_lab, new_lab_id );
        end
      end
      
      %   now add the remaining new labels
      for i = 1:n_new
        str_lab = new_str_labs{i};
        
        new_lab_id = locator.randlab2( obj.loc, B.loc );
        
        swaplab( B.loc, get(B.labels, str_lab), new_lab_id );
        set( obj.labels, str_lab, new_lab_id );
      end
      
      %   most of the work is here
      append( obj.loc, B.loc );
      
      destroy( B );
    end
    
    function disp(obj)
      
      %   DISP -- Pretty-print the object's contents.
      %
      %     See also locator/locator, locator/getcats
      
      desktop_exists = usejava( 'desktop' );
      
      if ( desktop_exists )
        link_str = sprintf( '<a href="matlab:helpPopup %s">%s</a>' ...
          , class(obj), class(obj) );
      else
        link_str = class( obj );
      end
      
      if ( ~isvalid(obj) )
        fprintf( 'Handle to deleted %s instance.\n\n', link_str );
        return;
      end
      
      cats = strcats( obj );
      
      if ( numel(cats) == 0 )
        addtl_str = 'with 0 categories';
      else
        addtl_str = 'with labels:';
      end
      
      max_labs = 5;
      
      sz = numel( obj );
      sz_str = sprintf( '%d×1', sz );
      
      fprintf( '  %s %s %s', sz_str, link_str, addtl_str );
      
      if ( numel(cats) > 0 )
        fprintf( '\n' );
      end
      
      n_digits = cellfun( @numel, cats );
      
      max_n_digits = max( n_digits );
      
      for i = 1:numel(cats)
        c_cat = cats{i};
        
        labs = cellstr( strcombs(obj, c_cat) );
        
        amt_pad = max_n_digits - numel( c_cat );
        cat_space = repmat( ' ', 1, amt_pad );
        
        n_labs = numel( labs );
        n_disp = min( n_labs, max_labs );
        
        if ( desktop_exists )
          fprintf( '\n  %s<strong>%s</strong>:', cat_space, c_cat );
        else
          fprintf( '\n  %s%s:', cat_space, c_cat );
        end
        
        lab_str = strjoin( labs(1:n_disp), ', ' );
        
        if ( n_disp < n_labs )
          lab_str = sprintf( '%s ..', lab_str );
        end
        
        lab_str = sprintf( '[%s]', lab_str );
        
        fprintf( ' %s', lab_str );
      end
      
      fprintf( '\n\n' );
    end
    
    function destroy(obj)
      
      %   DESTROY -- Delete properties and free memory.
      %
      %     See also locator/destroy
      
      destroy( obj.loc );
      destroy( obj.labels );
      destroy( obj.categories );
    end
    
    function [C, all_cats] = cellstr(obj)
      
      %   CELLSTR -- Convert to a cell array of strings.
      %
      %     OUT:
      %       - `c` (cell array of strings)
      %       - `all_cats` (cell array of strings)
      
      C = zeros( size(obj, 1), ncats(obj) );
      all_cats = getcats( obj );
      
      for i = 1:size(C, 2)
        C(:, i) = fullcat( obj.loc, all_cats(i) );
      end
      
      C = get( obj.labels, C );
      
      if ( nargout > 1 )
        all_cats = get( obj.categories, all_cats );
      end
    end
    
    function [C, cats] = categorical(obj)
      
      %   CATEGORICAL -- Convert to categorical array.
      %
      %     C = categorical( obj ) converts the object to a categorical
      %     array.
      %
      %     [C, cats] = ... also returns `cats`, a 1xN cell array of
      %     strings that identify the columns of `C`.
      %
      %     OUT:
      %       - `c` (categorical)
      %       - `cats` (cell array of strings)
      
      [C, cats] = cellstr( obj );
      C = categorical( C );      
    end
  end
  
  methods (Static = true)
    
    function obj = from(varargin)
      
      %   FROM -- Convert to labeler from compatible source.
      %
      %     See also locator/from
      %
      %     OUT:
      %       - `obj` (labeler)
      
      [loc, c, l] = locator.from( varargin{:} );
      
      obj = labeler( loc, c, l );
    end
  end
end