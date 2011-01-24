VBFA_setNk <-
structure(function(self, Nk)
{
  Nk = as.integer(Nk); 
  
  if(length(Nk) > 1) {
    warning("using only the first element of Nk");
  };
  
  ;.Call('R_swig_VBFA_setNk', self, Nk, PACKAGE='peer');
  
}
, returnType = "void", inputTypes = c("_p_PEER__cVBFA", "integer"
), class = c("SWIGFunction", "character"))
