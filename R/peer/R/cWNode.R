cWNode <-
function(...) {
  argtypes <- mapply(class, list(...));
  argv <- list(...);
  argc <- length(argtypes);
# dispatch functions 2
  if (argc == 0) {
    f <- cWNode__SWIG_0; 
  } else if (argc == 1) {
    if (extends(argtypes[1], '_p_PMatrix')) {
      f <- cWNode__SWIG_1; 
    }
  };
  f(...);
}

