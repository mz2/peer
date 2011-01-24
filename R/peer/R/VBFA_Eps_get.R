VBFA_Eps_get <-
structure(function(self)
{
  ;ans = .Call('R_swig_VBFA_Eps_get', self, PACKAGE='peer');
  class(ans) <- "_p_PEER__cEpsNode";
  
  ans
  
}
, returnType = "_p_PEER__cEpsNode", inputTypes = "_p_PEER__cVBFA", class = c("SWIGFunction", 
"character"))
