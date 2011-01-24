cWNode_E2S_get <-
structure(function(self, .copy = FALSE)
{
  ;ans = .Call('R_swig_cWNode_E2S_get', self, as.logical(.copy), PACKAGE='peer');
  class(ans) <- "_p_PMatrix";
  
  ans
  
}
, returnType = "_p_PMatrix", inputTypes = "_p_PEER__cWNode", class = c("SWIGFunction", 
"character"))
