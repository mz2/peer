cXNode_prior_offset_get <-
structure(function(self, .copy = FALSE)
{
  ;ans = .Call('R_swig_cXNode_prior_offset_get', self, as.logical(.copy), PACKAGE='peer');
  class(ans) <- "_p_PMatrix";
  
  ans
  
}
, returnType = "_p_PMatrix", inputTypes = "_p_PEER__cXNode", class = c("SWIGFunction", 
"character"))
