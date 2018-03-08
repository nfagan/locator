function loc_test_assert_depends_present()

if ( isempty(which('get_example_container')) )
  error( ['Tests depend on the `global` repository, available' ...
    , ' at: https://github.com/nfagan/global. Download or clone the repository' ...
    , ' and add it to matlab''s search path.'] );
end

end