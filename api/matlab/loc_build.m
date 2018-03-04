function loc_build( mex_func, sub_dirs )

if ( nargin < 2 )
  sub_dirs = { '' };
else
  if ( ~iscell(sub_dirs) ), sub_dirs = { sub_dirs }; end
end

api_dir = fileparts( which(mfilename) );

if ( ispc() )
  pathsep = '\';
else
  pathsep = '/';
end

api_dir_search = strjoin( {'api', 'matlab'}, pathsep );
api_dir_index = strfind( api_dir, api_dir_search );

repo_dir = api_dir(1:api_dir_index-1);

out_dir = fullfile( api_dir, sub_dirs{:} );

mex_func_path = fullfile( out_dir, mex_func );
loc_lib_dir = fullfile( repo_dir, 'lib' );
loc_include_dir= fullfile( repo_dir, 'include' );
loc_lib_name = 'locator';

build_cmd = sprintf( '-v COPTIMFLAGS="-O3 -fwrapv -DNDEBUG" CXXOPTIMFLAGS="-O3 -fwrapv -DNDEBUG" %s -I%s -l%s -L%s -outdir %s' ...
  , mex_func_path, loc_include_dir, loc_lib_name, loc_lib_dir, out_dir );

eval( sprintf('mex %s', build_cmd) );

end