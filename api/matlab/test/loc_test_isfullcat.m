function loc_test_isfullcat()

loc_test_assert_depends_present();

sp = get_labels( get_example_container() );

loc = locator.from( sp );

assert( all(isfullcat(loc, getcats(loc))) );

loc.resize( numel(loc) + 100 );

assert( ~any(isfullcat(loc, getcats(loc))) );

locb = requirecat( locator(), 0 );
requirecat( locb, 1 );

assert( all(isfullcat(locb, getcats(locb))) );

initcat( locb, 2, ones(100, 1) );

assert( sum(isfullcat(locb, getcats(locb))) == 1 && isfullcat(locb, 2) );

end