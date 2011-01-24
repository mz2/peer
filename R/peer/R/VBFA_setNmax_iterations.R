VBFA_setNmax_iterations <-
structure(function(self, Nmax_iterations)
{
  Nmax_iterations = as.integer(Nmax_iterations); 
  
  if(length(Nmax_iterations) > 1) {
    warning("using only the first element of Nmax_iterations");
  };
  
  ;.Call('R_swig_VBFA_setNmax_iterations', self, Nmax_iterations, PACKAGE='peer');
  
}
, returnType = "void", inputTypes = c("_p_PEER__cVBFA", "integer"
), class = c("SWIGFunction", "character"))
