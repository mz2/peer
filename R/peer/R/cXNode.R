cXNode <-
function(...) {
  argtypes <- mapply(class, list(...));
  argv <- list(...);
  argc <- length(argtypes);
# dispatch functions 3
  if (argc == 0) {
    f <- cXNode__SWIG_0; 
  } else if (argc == 1) {
    if (extends(argtypes[1], '_p_PMatrix')) {
      f <- cXNode__SWIG_1; 
    }
  } else if (argc == 3) {
    if (extends(argtypes[1], '_p_PMatrix') && extends(argtypes[2], '_p_PMatrix') && extends(argtypes[3], '_p_PMatrix')) {
      f <- cXNode__SWIG_2; 
    }
  };
  f(...);
}

