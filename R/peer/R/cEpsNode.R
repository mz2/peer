cEpsNode <-
function(...) {
  argtypes <- mapply(class, list(...));
  argv <- list(...);
  argc <- length(argtypes);
# dispatch functions 2
  if (argc == 0) {
    f <- cEpsNode__SWIG_0; 
  } else if (argc == 4) {
    if (extends(argtypes[1], 'integer') && is.numeric(argv[[2]]) && is.numeric(argv[[3]]) && extends(argtypes[4], '_p_PMatrix')) {
      f <- cEpsNode__SWIG_1; 
    }
  };
  f(...);
}

