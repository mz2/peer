VBFA_setAdd_mean <-
structure(function(self, add_mean)
{
  add_mean = as.logical(add_mean);
  ;.Call('R_swig_VBFA_setAdd_mean', self, add_mean, PACKAGE='peer');
  
}
, returnType = "void", inputTypes = c("_p_PEER__cVBFA", "logical"
), class = c("SWIGFunction", "character"))
