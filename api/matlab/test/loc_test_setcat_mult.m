function loc_test_setcat_mult()

sz = 1e5;
n_set = 10;

for i = 1:1e3
  x = locator();
  
  cat = 10;

  requirecat( x, cat );
  
  ind = true( sz, 1 );
  
  setcat( x, cat, 0, ind );
  
  labs = sort( randperm(numel(x), n_set) );
  
  setcat( x, cat, labs, trueat(x, labs) );
  
  assert( all(arrayfun(@(y) find(x, y) == y, labs)) );
  
  assert( nlabs(x) == numel(labs) + 1 );
end

end