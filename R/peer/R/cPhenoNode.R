cPhenoNode <-
function(...) {
  argtypes <- mapply(class, list(...));
  argv <- list(...);
  argc <- length(argtypes);
# dispatch functions 2
  if (argc == 0) {
    f <- cPhenoNode__SWIG_0; 
  } else if (argc == 2) {
    if (extends(argtypes[1], '_p_PMatrix') && extends(argtypes[2], '_p_PMatrix')) {
      f <- cPhenoNode__SWIG_1; 
    }
  };
  f(...);
}

