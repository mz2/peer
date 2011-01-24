setVerbose <-
structure(function(verbose)
{
  verbose = as.integer(verbose); 
  
  if(length(verbose) > 1) {
    warning("using only the first element of verbose");
  };
  
  ;.Call('R_swig_setVerbose', verbose, PACKAGE='peer');
  
}
, returnType = "void", inputTypes = "integer", class = c("SWIGFunction", 
"character"))
