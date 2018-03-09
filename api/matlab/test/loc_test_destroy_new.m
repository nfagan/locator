function loc_test_destroy_new(old_instances)

c_instances = locator.instances();
loc_destroy( setdiff(c_instances, old_instances) );

end