classdef multimap
  
  properties (Access = private)
    id;
  end
  
  methods (Access = public)    
    function obj = multimap(map_or_id)
      
      %   MULTIMAP -- Instantiate a multimap object.
      %
      %     obj = multimap() creates an empty multimap object.
      %
      %     MULTIMAP objects map string keys to uint32 values, and 
      %     values to keys. In this way, lookup is constant time in either 
      %     direction.
      %
      %     MULTIMAP objects are meant to be used along side LOCATOR
      %     objects, in order to efficiently switch between string and
      %     integer representations of labels and categories.
      %
      %     See also multimap/set, multimap/get, locator/locator
      
      if ( nargin == 0 || isa(map_or_id, 'containers.Map') )
        obj.id = loc_multimap_api( loc_multimap_opcodes('create') );
      end
      
      if ( nargin == 1 && isa(map_or_id, 'uint32') )
        assert( numel(map_or_id) == 1 && any(multimap.instances == map_or_id) ...
          , 'Invalid map id.' );
        obj.id = map_or_id;
        return;
      end
      
      if ( nargin == 1 )
        assert( isa(map_or_id, 'containers.Map'), ['Input must be a containers.Map' ...
          , 'object; was "%s".'], class(map_or_id) );
        kinds = { map_or_id.KeyType, map_or_id.ValueType };
        assert( sum(strcmp(kinds, 'uint32')) == 1 && sum(strcmp(kinds, 'char')) == 1 ...
          , ['map object must have a KeyType of char and value type of uint32' ...
          , ' or vice versa.'] );
        keys = map_or_id.keys();
        cellfun( @(x) set(obj, x, map_or_id(x)), keys, 'un', false );
      end
    end
    
    function tf = isvalid(obj)
      
      %   ISVALID -- True if the multimap has not been destroyed.
      %
      %     OUT:
      %       - `tf` (logical)
      
      tf = any( multimap.instances == obj.id );
    end
    
    function tf = contains(obj, kv)
      
      %   CONTAINS -- True if the multimap contains the key or value.
      %
      %     IN:
      %       - `kv` (uint32, char)
      %     OUT:
      %       - `tf` (logical)
      
      if ( isnumeric(kv) )
        kv = uint32( kv );
      end
      
      tf = loc_multimap_api( loc_multimap_opcodes('contains'), obj.id, kv );
    end
    
    function k = keys(obj)
      
      %   KEYS -- Get the char keys of the multimap.
      %
      %     See also multimap/get, multimap/set, multimap/values
      %
      %     OUT:
      %       - `k` (cell array of strings)
      
      k = loc_multimap_api( loc_multimap_opcodes('keys'), obj.id );      
    end
    
    function v = values(obj)
      
      %   VALUES -- Get the uint32 values of the multimap.
      %
      %     See also multimap/get, multimap/set, multimap/keys
      %
      %     OUT:
      %       - `v` (uint32)
      
      v = loc_multimap_api( loc_multimap_opcodes('values'), obj.id );  
    end
    
    function id = getid(obj)
      
      %   GETID -- Get the id of the object.
      %
      %     See also multimap/multimap
      %
      %     OUT:
      %       - `id` (uint32)
      
      id = obj.id;      
    end
    
    function obj = subsasgn(obj, s, values)
      
      %   SUBSASGN -- Assign value to key.
      %
      %     obj('example') = 1 binds 1 to 'example'.
      %     obj(1) = 'example' does the same.
      %
      %     Assignment in this way is slower than calling `set()` directly.
      %
      %     See also multimap/subsref, multimap/set, multimap/get
      
      assert( numel(s) == 1 && strcmp(s.type, '()') && numel(s.subs) == 1 ...
        , 'Invalid assignment signature.' );
      
      set( obj, s.subs{1}, values );
    end
    
    function out = subsref(obj, s)
      
      %   SUBSREF -- Retrieve value associated with key.
      %
      %     out = obj('example') gets the value associated with 'example'.
      %     out = obj(1) gets the value associated with 1.
      %
      %     out = obj({'example', 'example2'}) returns a 1x2 array of
      %     integer values associated with string keys.
      %
      %     out = obj([1, 2]) returns a 1x2 cell array of string values
      %     associated with integer keys.
      %
      %     Reference in this way is slower than calling `get()` directly.
      %
      %     See also multimap/subsref, multimap/set, multimap/get
      
      assert( numel(s) == 1 && strcmp(s.type, '()') && numel(s.subs) == 1 ...
        , 'Invalid reference signature.' );
      
      referent = s.subs{1};
      
      %   obj([1, 2, 3])
      if ( isnumeric(referent) && numel(referent) > 1 )
        out = arrayfun( @(x) get(obj, x), s.subs{1}, 'un', false );
        return;
      end
      
      %   obj({'eg1', 'eg2'})
      if ( iscellstr(referent) )
        out = cellfun( @(x) get(obj, x), s.subs{1} );
        return;
      end
      
      %   obj('eg') or obj(1)
      out = get( obj, referent );
    end
    
    function obj = set(obj, key, value)
      
      %   SET -- Assign value to key, and key to value.
      %
      %     set( obj, 'example', uint32(0) ) associates 'example' with 0.
      %
      %     See also multimap/multimap, multimap/get
      %
      %     IN:
      %       - `key` (uint32, char)
      %       - `value` (uint32, char)
      
      if ( isnumeric(value) )
        value = uint32( value );
      end
      
      if ( isnumeric(key) )
        key = uint32( key );
      end
      
      loc_multimap_api( loc_multimap_opcodes('insert'), obj.id, key, value );
    end
    
    function value = get(obj, key)
      
      %   GET -- Retrieve value.
      %
      %     See also multimap/set
      %
      %     IN:
      %       - `key` (uint32, char)
      %     OUT:
      %       - `value` (uint32, char)
      
      if ( isnumeric(key) )
        key = uint32( key );
      end
      
      opcode = loc_multimap_opcodes( 'at' );
      value = loc_multimap_api( opcode, obj.id, key );
    end
    
    function destroy(obj)
      
      %   DESTROY -- Delete instance and free memory.
      %
      %     See also multimap/destroyall, multimap/multimap
      
      opcode = loc_multimap_opcodes( 'destroy' );
      loc_multimap_api( opcode, obj.id );
    end
    
    function new_map = copy(obj)
      
      %   COPY -- Create new instance from current instance.
      %
      %     OUT:
      %       - `new_map` (multimap)
    
      opcode = loc_multimap_opcodes( 'copy' );
      
      new_map = multimap( loc_multimap_api(opcode, obj.id) );
    end
    
    function out = map(obj, flip)
      
      %   MAP -- Convert to a containers.Map instance.
      %
      %     See also multimap/multimap
      %
      %     IN:
      %       - `flip` (logical) |OPTIONAL| -- If true, output map is
      %       uint32 -> string. If false, output map is string -> uint32.
      %       Default is false.
      %     OUT:
      %       - `out` (containers.Map)
      
      if ( nargin < 2 )
        flip = false;
      end
      
      if ( flip )
        out = containers.Map( values(obj), keys(obj) );
        return;
      end
      
      out = containers.Map( keys(obj), values(obj) );      
    end
    
    function disp(obj)
      
      %   DISP -- Pretty-print the object's contents.
      %
      %     See also multimap/multimap, multimap/destroy
      
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
      
      k = keys( obj );
      sz = numel( k );
      
      sz_str = sprintf( '%d×1', sz );
      
      if ( sz == 0 )
        addtl_str = '';
      else
        addtl_str = 'with keys: ';
      end
      
      fprintf( '  %s %s %s', sz_str, link_str, addtl_str );
      
      if ( sz > 0 )
        fprintf( '\n' );
      end
      
      n_disp = min( numel(k), 5 );
      
      for i = 1:n_disp
        if ( desktop_exists )
          key_str = sprintf( '<strong>%s</strong>', k{i} );
        else
          key_str = k{i};
        end
        
        fprintf( '\n    %s: %d', key_str, get(obj, k{i}) );
      end
      
      if ( n_disp < numel(k) )
        fprintf( '\n    .. ' );
      end
      
      fprintf( '\n\n' );
    end
  end
  
  methods (Static = true)
    
    function out = instances()
      
      %   INSTANCES -- Get ids of all active multimaps.
      %
      %     See also multimap/multimap
      %
      %     OUT:
      %       - `ids` (uint32)
      
      opcode = loc_multimap_opcodes( 'instances' );
      out = loc_multimap_api( opcode );
    end
    
    function destroyall()
      
      %   DESTROYALL -- Delete all multimaps.
      %
      %     See also multimap/destroy
      
      opcode = loc_multimap_opcodes( 'destroy' );
      loc_multimap_api( opcode );
    end
  end
end