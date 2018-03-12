classdef multimap
  
  properties (Access = private)
    kv;
    vk;
  end
  
  properties (Constant = true, Access = private)
    key_t = 'char';
    value_t = 'uint32';
  end
  
  methods (Access = public)    
    function obj = multimap()
      obj.kv = containers.Map( 'keytype', obj.key_t, 'valuetype', obj.value_t);
      obj.vk = containers.Map( 'keytype', obj.value_t, 'valuetype', obj.key_t );
    end
    
    function tf = isKey(obj, k)
      
      %   ISKEY -- True if key `k` exists.
      %
      %     IN:
      %       - `k` (key_t, value_t)
      %     OUT:
      %       - `tf` (logical)
      
      tf = obj.kv.isKey( k ) || obj.vk.isKey( k );
    end
    
    function obj = subsasgn(obj, s, value)
      
      %   SUBSASGN -- Assign values to the object.
      
      proceed = true;
      
      try
        switch ( s(1).type )
          case '()'
            assert( numel(s) == 1, ...
              'Nested assignments with ''()'' are illegal.' );
            subs = s(1).subs;
            
            referent = subs{1};
            
            if ( isa(referent, obj.key_t) )
              obj.kv(referent) = value;
              obj.vk(value) = referent;
              proceed = false;
            end
            
            if ( proceed && isa(referent, obj.value_t) )
              obj.vk(referent) = value;
              obj.kv(value) = referent;
              proceed = false;
            end
            
            if ( proceed )
              error( 'Assignment with key of type "%s" is not supported.' ...
                , class(referent) );
            end
          otherwise
            error( 'Assignment via ''%s'' is not supported', s(1).type );
        end
      catch err
        throwAsCaller( err );
      end
    end
    
    function varargout = subsref(obj, s)
      
      %   SUBSREF -- Get properties and call methods of the object.
      
      try
        subs = s(1).subs;
        type = s(1).type;

        s(1) = [];

        proceed = true;

        switch ( type )
          case '.'
            %   if the ref is the name of a multimap method, call the method
            %   on the Container object (with whatever other inputs are
            %   passed), and return
            if ( proceed && any(strcmp(methods(obj), subs)) )
              func = eval( sprintf('@%s', subs) );
              %   if the ref is to a method, but is called without (), an
              %   error is thrown. E.g., Container.eq -> error ...
              if ( numel(s) == 0 )
                s(1).subs = {};
              end
              inputs = [ {obj} {s(:).subs{:}} ];
              %   assign `out` to the output of func() and return
              [varargout{1:nargout()}] = func( inputs{:} );
              return; %   note -- in this case, we do not proceed
            end
            if ( proceed )
              error( 'No properties or methods matched the name ''%s''', subs );
            end
          case '()'
            nsubs = numel( subs );
            %   ensure we're not doing x()
            assert( nsubs ~= 0, ['Attempted to reference a variable' ...
              , ' as if it were a function.'] );
            %   ensure we're not doing x(1, 2, 3)
            assert( nsubs == 1, ['Multidimensional indexing is not' ...
              , ' supported.'] );
            %   use a numeric index
            if ( isa(subs{1}, obj.key_t) )
              out = obj.kv(subs{1});
              proceed = false;
            end
            if ( proceed && isa(subs{1}, obj.value_t) )
              out = obj.vk(subs{1});
              proceed = false;
            end
            %   otherwise, we've attempted to pass an illegal type to the
            %   index
            if ( proceed )
              error( '() Referencing with values of class ''%s'' is not supported.', ...
                class(subs{1}) );
            end
          otherwise
            error( 'Referencing with ''%s'' is not supported', type );
        end

        if ( isempty(s) )
          varargout{1} = out;
          return;
        end
        %   continue referencing if this is a nested reference, e.g.
        %   obj.labels.labels
        [varargout{1:nargout()}] = subsref( out, s );
      catch err
        throwAsCaller( err );
      end
    end
  end
  
end