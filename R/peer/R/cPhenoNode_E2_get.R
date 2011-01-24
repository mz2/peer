cPhenoNode_E2_get <-
structure(function(self, .copy = FALSE)
{
  ;ans = .Call('R_swig_cPhenoNode_E2_get', self, as.logical(.copy), PACKAGE='peer');
  class(ans) <- "_p_PMatrix";
  
  ans
  
}
, returnType = "_p_PMatrix", inputTypes = "_p_PEER__cPhenoNode", class = c("SWIGFunction", 
"character"))
