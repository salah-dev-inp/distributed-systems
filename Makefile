dep0 : dep1 dep2 dep3
  echo "dep0"
dep1 : dep4 dep5
  echo "dep1"
dep2 : dep6
  echo "dep2"
dep4 : dep7 dep8
  echo "dep4"
dep5 : dep9 dep10
  echo "dep5"
dep10 : dep11
  echo "dep10"
dep11 : dep12
  echo "dep12"
dep12 : dep13
  echo "dep13"
dep13 : dep14
  echo "dep14"